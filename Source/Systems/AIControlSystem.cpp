#include "AIControlSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"

#include <cmath>
#include <vector>

namespace
{
constexpr auto  CHASE_DISTANCE_BASE    = 450.0f;
constexpr auto  SHOOT_DISTANCE_BASE    = 100.0f;
constexpr auto  SEPARATION_RADIUS      = 60.0f;
constexpr auto  SEPARATION_WEIGHT      = 1.45f;
constexpr auto  OBSTACLE_LOOKAHEAD     = 90.0f;
constexpr auto  OBSTACLE_PROBE_RADIUS  = 18.0f;
constexpr auto  OBSTACLE_WEIGHT        = 2.2f;
constexpr auto  APPROACH_SPREAD        = 45.0f;
constexpr auto  ORBIT_RADIUS_FACTOR    = 0.75f;
constexpr auto  BOOST_ENGAGE_DISTANCE  = 130.0f;
constexpr auto  BOOST_DURATION         = 1.4f;
constexpr auto  BOOST_COOLDOWN         = 4.0f;
constexpr auto  FRUSTRATION_BASE       = 2.2f;
constexpr auto  RETARGET_COOLDOWN      = 1.8f;
constexpr auto  LEAD_TIME_MAX          = 1.0f;
constexpr auto  PATROL_THROTTLE        = 0.35f;
constexpr auto  MAX_PURSUERS           = 3;
constexpr auto  LEADER_SEARCH_RADIUS   = 160.0f;
constexpr float BRAKING_DISTANCE       = 55.0f;
constexpr float RETARGET_SCORE_MARGIN  = 35.0f;
constexpr float MANEUVER_MIN_TIME      = 0.8f;
const auto      SHOOT_ANGLE_TRESHOLD   = glm::cos(glm::radians(20.0f));

float EntityNoise(fr::Entity entity, uint32_t salt = 0)
{
    auto h = static_cast<uint32_t>(entity) * 747796405u + salt * 2891336453u;
    h      = (h ^ (h >> 16)) * 0x45d9f3bu;
    h      = (h ^ (h >> 16)) * 0x45d9f3bu;
    return static_cast<float>(h & 0xFFFFu) / 65535.0f;
}

float ChaseDistanceFor(const AIControlledComponent& ai)
{
    return CHASE_DISTANCE_BASE * (0.75f + ai.aggression * 0.5f);
}

float ShootDistanceFor(const AIControlledComponent& ai)
{
    return SHOOT_DISTANCE_BASE * ai.shootRangeScale;
}

float FrustrationThresholdFor(const AIControlledComponent& ai)
{
    return FRUSTRATION_BASE * (1.35f - ai.aggression * 0.5f);
}

float ScoreTarget(const TransformComponent& self, const glm::vec3& candidatePos, float chaseDistance)
{
    const auto  toCandidate = candidatePos - self.position;
    const float distance    = glm::length(toCandidate);
    if (distance < 0.001f)
        return -1.0e6f;

    const auto  direction = toCandidate / distance;
    const float facing    = glm::dot(self.GetForwardDirection(), direction);
    return facing * chaseDistance - distance;
}

glm::vec3 SafeNormalize(const glm::vec3& v, const glm::vec3& fallback = WORLD_FORWARD)
{
    const float len = glm::length(v);
    if (len < 0.001f)
        return fallback;
    return v / len;
}

glm::vec3 LateralAxis(const glm::vec3& toTarget)
{
    glm::vec3 lateral = glm::cross(toTarget, WORLD_UP);
    if (glm::length(lateral) < 0.001f)
        lateral = glm::cross(toTarget, WORLD_RIGHT);
    return SafeNormalize(lateral);
}
} // namespace

AIControlSystem::AIControlSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<OctreeSystem>& octreeSystem) :
    System(registry), mOctree(octreeSystem)
{
}

void AIControlSystem::Update(float deltaTime)
{
    mRegistry->CreateMutation()
        ->EachAsync<AIControlledComponent, TransformComponent, SquadComponent, LaserGunComponent,
                    SpaceShipControlComponent>(
            [this, deltaTime](fr::Entity entity, AIControlledComponent& aiControlled, TransformComponent& transform,
                              SquadComponent& squad, LaserGunComponent& laserGun,
                              SpaceShipControlComponent& spaceShipControl) {
                spaceShipControl.throttle       = 1.0f;
                spaceShipControl.boostFactor    = 1.0f;
                spaceShipControl.pitchTorque    = 0.0f;
                spaceShipControl.yawTorque      = 0.0f;
                spaceShipControl.rollTorque     = 0.0f;
                spaceShipControl.volatileTorque = false;

                if (aiControlled.retargetCooldown > 0.0f)
                    aiControlled.retargetCooldown -= deltaTime;

                switch (aiControlled.behaviour)
                {
                    case Behaviour::Patrol:
                        Patrol(entity, aiControlled, squad, transform, spaceShipControl);
                        break;
                    case Behaviour::Chase:
                        Chase(entity, aiControlled, spaceShipControl, squad, transform, laserGun, deltaTime);
                        break;
                    case Behaviour::Flee:
                        Flee(entity, aiControlled, spaceShipControl, transform, deltaTime);
                        break;
                }
            });
}

void AIControlSystem::Patrol(fr::Entity entity, AIControlledComponent& aiControlled, SquadComponent& squad,
                             TransformComponent& transform, SpaceShipControlComponent& spaceShipControl)
{
    spaceShipControl.throttle = PATROL_THROTTLE * (0.7f + aiControlled.aggression * 0.5f);

    const float wanderPhase = EntityNoise(entity, 7) * glm::two_pi<float>();
    const float wanderAmp   = 0.25f + EntityNoise(entity, 13) * 0.35f;
    spaceShipControl.yawTorque   = std::sin(wanderPhase) * wanderAmp;
    spaceShipControl.pitchTorque = std::cos(wanderPhase * 0.7f) * wanderAmp * 0.5f;
    spaceShipControl.rollTorque  = glm::dot(transform.GetRightDirection(), WORLD_UP) * 0.5f;

    const auto obstacle = ComputeObstacleAvoidance(entity, transform);
    if (glm::dot(obstacle, obstacle) > 0.001f)
    {
        ApplySteering(spaceShipControl, transform, SafeNormalize(transform.GetForwardDirection() + obstacle),
                      transform.GetForwardDirection());
    }

    if (const auto leaderTarget = FindSquadLeaderTarget(entity, aiControlled, squad, transform); leaderTarget)
    {
        aiControlled.target      = leaderTarget.value();
        aiControlled.behaviour   = Behaviour::Chase;
        aiControlled.maneuver    = CombatManeuver::Orbit;
        aiControlled.frustration = 0.0f;
        aiControlled.maneuverTime = 0.0f;
        return;
    }

    const auto target = FindBestTarget(entity, aiControlled, squad, transform);
    if (!target.has_value())
        return;

    aiControlled.target       = target.value();
    aiControlled.behaviour    = Behaviour::Chase;
    aiControlled.maneuver     = CombatManeuver::Pursue;
    aiControlled.squadLeader  = 0;
    aiControlled.frustration  = 0.0f;
    aiControlled.maneuverTime = 0.0f;
}

void AIControlSystem::Chase(fr::Entity entity, AIControlledComponent& aiControlled,
                            SpaceShipControlComponent& spaceShipControl, const SquadComponent& squad,
                            TransformComponent& transform, LaserGunComponent& laserGun, float deltaTime)
{
    bool        targetAlive   = false;
    const float chaseDistance = ChaseDistanceFor(aiControlled);
    const float shootRange    = ShootDistanceFor(aiControlled);

    mRegistry->TryGetComponents<TransformComponent, SquadComponent>(
        aiControlled.target, [&](TransformComponent& targetTransform, const SquadComponent& targetSquad) {
            targetAlive = true;

            if (targetSquad.squad == squad.squad)
            {
                aiControlled.behaviour = Behaviour::Patrol;
                aiControlled.target    = 0;
                aiControlled.squadLeader = 0;
                laserGun.triggered     = false;
                return;
            }

            const auto distanceVector = targetTransform.position - transform.position;
            const auto distance       = glm::length(distanceVector);

            if (distance > chaseDistance || distance < 0.001f)
            {
                aiControlled.behaviour = Behaviour::Patrol;
                aiControlled.target    = 0;
                aiControlled.squadLeader = 0;
                laserGun.triggered     = false;
                return;
            }

            const int pursuers = CountPursuers(aiControlled.target, squad, entity, targetTransform.position);
            if (pursuers > MAX_PURSUERS && aiControlled.retargetCooldown <= 0.0f && !aiControlled.preferLead)
            {
                const auto alternative = FindBestTarget(entity, aiControlled, squad, transform, aiControlled.target);
                if (alternative.has_value())
                {
                    aiControlled.target           = alternative.value();
                    aiControlled.squadLeader      = 0;
                    aiControlled.frustration      = 0.0f;
                    aiControlled.retargetCooldown = RETARGET_COOLDOWN;
                    aiControlled.maneuver         = CombatManeuver::Pursue;
                    laserGun.triggered            = false;
                    return;
                }
            }

            const auto toTarget   = distanceVector / distance;
            const auto forward    = transform.GetForwardDirection();
            const auto forwardDot = glm::dot(forward, toTarget);

            if (forwardDot < 0.25f)
                aiControlled.frustration += deltaTime * (1.2f - aiControlled.aggression * 0.4f);
            else
                aiControlled.frustration = glm::max(0.0f, aiControlled.frustration - deltaTime * 0.6f);

            if (aiControlled.frustration >= FrustrationThresholdFor(aiControlled) &&
                aiControlled.retargetCooldown <= 0.0f)
            {
                const auto currentScore = ScoreTarget(transform, targetTransform.position, chaseDistance);
                const auto alternative  = FindBestTarget(entity, aiControlled, squad, transform, aiControlled.target);

                if (alternative.has_value())
                {
                    bool switchTarget = false;
                    mRegistry->TryGetComponents<TransformComponent>(alternative.value(), [&](TransformComponent& alt) {
                        switchTarget =
                            ScoreTarget(transform, alt.position, chaseDistance) > currentScore + RETARGET_SCORE_MARGIN;
                    });

                    if (switchTarget)
                    {
                        aiControlled.target           = alternative.value();
                        aiControlled.squadLeader      = 0;
                        aiControlled.frustration      = 0.0f;
                        aiControlled.retargetCooldown = RETARGET_COOLDOWN;
                        aiControlled.maneuver         = CombatManeuver::Pursue;
                        laserGun.triggered            = false;
                        return;
                    }
                }

                if (forwardDot < 0.0f)
                {
                    aiControlled.maneuver     = CombatManeuver::Reposition;
                    aiControlled.maneuverTime = 0.0f;
                }

                aiControlled.frustration      = FrustrationThresholdFor(aiControlled) * 0.4f;
                aiControlled.retargetCooldown = RETARGET_COOLDOWN * 0.5f;
            }

            auto desired =
                ComputeCombatAim(entity, aiControlled, transform, targetTransform, toTarget, distance, shootRange,
                                 deltaTime);

            const auto separation = ComputeSeparation(entity, squad, transform);
            if (glm::dot(separation, separation) > 0.001f)
                desired = SafeNormalize(desired + separation * SEPARATION_WEIGHT, desired);

            const auto obstacle = ComputeObstacleAvoidance(entity, transform);
            if (glm::dot(obstacle, obstacle) > 0.001f)
                desired = SafeNormalize(desired + obstacle * OBSTACLE_WEIGHT, desired);

            glm::vec3 faceDirection = toTarget;
            if (aiControlled.maneuver == CombatManeuver::Reposition)
                faceDirection = desired;

            ApplySteering(spaceShipControl, transform, desired, faceDirection);

            const float alignment = glm::max(0.0f, glm::dot(forward, desired));
            float       distanceFactor =
                glm::clamp(distance / BRAKING_DISTANCE, 0.35f, 1.0f);

            if (aiControlled.maneuver == CombatManeuver::Orbit || aiControlled.maneuver == CombatManeuver::Strafe)
                distanceFactor = glm::mix(0.55f, 0.95f, aiControlled.orbitBias);
            else if (aiControlled.maneuver == CombatManeuver::Reposition)
                distanceFactor = 1.0f;

            spaceShipControl.throttle = glm::mix(0.4f, 1.0f, alignment) * distanceFactor;

            const bool canShoot = distance <= shootRange && glm::dot(forward, toTarget) > SHOOT_ANGLE_TRESHOLD &&
                                  aiControlled.maneuver != CombatManeuver::Reposition;
            laserGun.triggered = canShoot;

            UpdateBoost(aiControlled, spaceShipControl, alignment, distance, deltaTime);
        });

    if (!targetAlive)
    {
        aiControlled.behaviour   = Behaviour::Patrol;
        aiControlled.target      = 0;
        aiControlled.squadLeader = 0;
        laserGun.triggered       = false;
    }
}

void AIControlSystem::Flee(fr::Entity entity, AIControlledComponent& aiControlled,
                           SpaceShipControlComponent& spaceShipControl, TransformComponent& transform, float deltaTime)
{
    aiControlled.fleeTime -= deltaTime;

    if (aiControlled.fleeTime <= 0.0f)
    {
        aiControlled.behaviour   = Behaviour::Patrol;
        aiControlled.target      = 0;
        aiControlled.squadLeader = 0;
        aiControlled.frustration = 0.0f;
        aiControlled.maneuver    = CombatManeuver::Pursue;
        return;
    }

    spaceShipControl.throttle    = 1.0f;
    spaceShipControl.boostFactor = BoostFactor;

    glm::vec3 fleeDir = transform.GetForwardDirection();
    bool      steered = false;

    if (aiControlled.target != 0)
    {
        mRegistry->TryGetComponents<TransformComponent>(aiControlled.target, [&](const TransformComponent& threat) {
            fleeDir = SafeNormalize(transform.position - threat.position, transform.GetForwardDirection());
            steered = true;
        });
    }

    const auto noise = transform.GetRightDirection() * (EntityNoise(entity, 17) * 2.0f - 1.0f) * 0.35f +
                       transform.GetUpDirection() * (EntityNoise(entity, 19) * 2.0f - 1.0f) * 0.2f;
    const auto obstacle = ComputeObstacleAvoidance(entity, transform);
    auto       desired  = SafeNormalize(fleeDir + noise + obstacle * OBSTACLE_WEIGHT, fleeDir);

    ApplySteering(spaceShipControl, transform, desired, desired);

    if (!steered)
        spaceShipControl.rollTorque = glm::dot(transform.GetRightDirection(), WORLD_UP);
}

std::optional<fr::Entity> AIControlSystem::FindBestTarget(fr::Entity entity, const AIControlledComponent& ai,
                                                          const SquadComponent& squad,
                                                          const TransformComponent& transform,
                                                          fr::Entity exclude) const
{
    thread_local std::vector<Particle> nearby;
    nearby.clear();

    const float chaseDistance = ChaseDistanceFor(ai);
    auto        queryParticle =
        Particle { .entity = entity, .transform = transform, .sphereCollider = { .radius = chaseDistance } };
    mOctree->Query(queryParticle, nearby);

    fr::Entity bestEntity = 0;
    float      bestScore  = -1.0e9f;

    for (const auto& other : nearby)
    {
        if (other.entity == entity || other.entity == exclude)
            continue;

        bool isOpponent = false;
        mRegistry->TryGetComponents<SquadComponent>(other.entity, [&](const SquadComponent& otherSquad) {
            isOpponent = squad.squad != otherSquad.squad;
        });

        if (!isOpponent)
            continue;

        const int pursuers = CountPursuers(other.entity, squad, entity, other.transform.position);
        if (pursuers >= MAX_PURSUERS && !ai.preferLead)
            continue;

        float score = ScoreTarget(transform, other.transform.position, chaseDistance);
        score -= static_cast<float>(pursuers) * 40.0f;
        score += ai.aggression * 20.0f;

        if (score > bestScore)
        {
            bestScore  = score;
            bestEntity = other.entity;
        }
    }

    if (bestEntity == 0)
        return std::nullopt;

    return bestEntity;
}

std::optional<fr::Entity> AIControlSystem::FindSquadLeaderTarget(fr::Entity entity, AIControlledComponent& ai,
                                                                 const SquadComponent& squad,
                                                                 const TransformComponent& transform) const
{
    thread_local std::vector<Particle> nearby;
    nearby.clear();

    auto queryParticle =
        Particle { .entity = entity, .transform = transform, .sphereCollider = { .radius = LEADER_SEARCH_RADIUS } };
    mOctree->Query(queryParticle, nearby);

    fr::Entity bestLeader = 0;
    float      bestRank   = -1.0f;
    fr::Entity leaderTarget = 0;

    for (const auto& other : nearby)
    {
        if (other.entity == entity)
            continue;

        mRegistry->TryGetComponents<AIControlledComponent, SquadComponent>(
            other.entity, [&](const AIControlledComponent& otherAi, const SquadComponent& otherSquad) {
                if (otherSquad.squad != squad.squad)
                    return;
                if (otherAi.behaviour != Behaviour::Chase || otherAi.target == 0)
                    return;
                if (!otherAi.preferLead && otherAi.aggression < ai.aggression + 0.15f)
                    return;

                const float rank = (otherAi.preferLead ? 1.0f : 0.0f) + otherAi.aggression;
                if (rank > bestRank)
                {
                    bestRank     = rank;
                    bestLeader   = other.entity;
                    leaderTarget = otherAi.target;
                }
            });
    }

    if (bestLeader == 0 || leaderTarget == 0)
        return std::nullopt;

    glm::vec3 targetPosition = transform.position;
    bool      targetValid    = false;
    mRegistry->TryGetComponents<TransformComponent, SquadComponent>(
        leaderTarget, [&](const TransformComponent& targetTransform, const SquadComponent& targetSquad) {
            targetValid    = targetSquad.squad != squad.squad;
            targetPosition = targetTransform.position;
        });

    if (!targetValid)
        return std::nullopt;

    const int pursuers = CountPursuers(leaderTarget, squad, entity, targetPosition);
    if (pursuers >= MAX_PURSUERS && !ai.preferLead)
        return std::nullopt;

    ai.squadLeader = bestLeader;
    ai.flankSign   = EntityNoise(entity, 29) < 0.5f ? -1.0f : 1.0f;
    return leaderTarget;
}

int AIControlSystem::CountPursuers(fr::Entity target, const SquadComponent& squad, fr::Entity self,
                                   const glm::vec3& targetPosition) const
{
    thread_local std::vector<Particle> nearby;
    nearby.clear();

    TransformComponent probeTransform {};
    probeTransform.position = targetPosition;

    auto queryParticle =
        Particle { .entity = target, .transform = probeTransform, .sphereCollider = { .radius = CHASE_DISTANCE_BASE } };
    mOctree->Query(queryParticle, nearby);

    int count = 0;
    for (const auto& other : nearby)
    {
        if (other.entity == self || other.entity == target)
            continue;

        mRegistry->TryGetComponents<AIControlledComponent, SquadComponent>(
            other.entity, [&](const AIControlledComponent& otherAi, const SquadComponent& otherSquad) {
                if (otherSquad.squad == squad.squad && otherAi.behaviour == Behaviour::Chase &&
                    otherAi.target == target)
                {
                    ++count;
                }
            });
    }

    return count;
}

glm::vec3 AIControlSystem::ComputeSeparation(fr::Entity entity, const SquadComponent& squad,
                                             const TransformComponent& transform) const
{
    thread_local std::vector<Particle> nearby;
    nearby.clear();

    auto queryParticle =
        Particle { .entity = entity, .transform = transform, .sphereCollider = { .radius = SEPARATION_RADIUS } };
    mOctree->Query(queryParticle, nearby);

    glm::vec3 separation { 0.0f };

    for (const auto& other : nearby)
    {
        if (other.entity == entity)
            continue;

        bool sameSquad = false;
        mRegistry->TryGetComponents<SquadComponent>(other.entity, [&](const SquadComponent& otherSquad) {
            sameSquad = squad.squad == otherSquad.squad;
        });

        if (!sameSquad)
            continue;

        const auto  offset = transform.position - other.transform.position;
        const float dist   = glm::length(offset);
        if (dist < 0.001f || dist >= SEPARATION_RADIUS)
            continue;

        const float weight = 1.0f - dist / SEPARATION_RADIUS;
        separation += (offset / dist) * (weight * weight);
    }

    return separation;
}

glm::vec3 AIControlSystem::ComputeObstacleAvoidance(fr::Entity entity, const TransformComponent& transform) const
{
    thread_local std::vector<Particle> nearby;
    nearby.clear();

    TransformComponent probe = transform;
    probe.position += transform.GetForwardDirection() * OBSTACLE_LOOKAHEAD;

    auto queryParticle =
        Particle { .entity = entity, .transform = probe, .sphereCollider = { .radius = OBSTACLE_PROBE_RADIUS } };
    mOctree->QueryKinematic(queryParticle, nearby);

    // Also sense obstacles near the ship itself.
    auto localProbe =
        Particle { .entity = entity, .transform = transform, .sphereCollider = { .radius = OBSTACLE_PROBE_RADIUS * 1.5f } };
    mOctree->QueryKinematic(localProbe, nearby);

    glm::vec3 avoidance { 0.0f };

    for (const auto& obstacle : nearby)
    {
        const auto  offset = transform.position - obstacle.transform.position;
        const float dist   = glm::length(offset);
        const float danger = obstacle.sphereCollider.radius + OBSTACLE_PROBE_RADIUS + 10.0f;
        if (dist < 0.001f || dist > danger + OBSTACLE_LOOKAHEAD)
            continue;

        const float weight = 1.0f - glm::clamp(dist / (danger + OBSTACLE_LOOKAHEAD), 0.0f, 1.0f);
        avoidance += SafeNormalize(offset) * (weight * weight);
    }

    return avoidance;
}

glm::vec3 AIControlSystem::ComputeCombatAim(fr::Entity entity, AIControlledComponent& ai,
                                            const TransformComponent& transform,
                                            const TransformComponent& targetTransform, const glm::vec3& toTarget,
                                            float distance, float shootRange, float deltaTime) const
{
    ai.maneuverTime += deltaTime;

    const bool canSwitch = ai.maneuverTime >= MANEUVER_MIN_TIME;
    const auto forward   = transform.GetForwardDirection();
    const auto forwardDot = glm::dot(forward, toTarget);

    if (canSwitch)
    {
        CombatManeuver next = ai.maneuver;

        if (ai.maneuver == CombatManeuver::Reposition)
        {
            if (distance > shootRange * 1.15f || forwardDot > 0.35f)
                next = CombatManeuver::Pursue;
        }
        else if (distance > shootRange * 1.35f)
        {
            next = CombatManeuver::Pursue;
        }
        else if (forwardDot < 0.05f)
        {
            next = CombatManeuver::Reposition;
        }
        else if (distance < shootRange * 0.85f)
        {
            next = (ai.orbitBias > 0.55f || ai.squadLeader != 0) ? CombatManeuver::Orbit : CombatManeuver::Strafe;
        }
        else
        {
            next = CombatManeuver::Pursue;
        }

        if (next != ai.maneuver)
        {
            ai.maneuver     = next;
            ai.maneuverTime = 0.0f;
        }
    }

    glm::vec3 aimPoint = targetTransform.position;
    mRegistry->TryGetComponents<RigidBodyComponent>(ai.target, [&](const RigidBodyComponent& body) {
        const float closingSpeed = glm::max(1.0f, glm::length(body.velocity));
        const float leadTime     = glm::clamp(distance / closingSpeed, 0.0f, LEAD_TIME_MAX);
        aimPoint += body.velocity * leadTime;
    });

    const auto lateral = LateralAxis(toTarget);
    const auto orbitUp = SafeNormalize(glm::cross(lateral, toTarget), WORLD_UP);
    const float flank  = ai.flankSign;

    switch (ai.maneuver)
    {
        case CombatManeuver::Orbit: {
            const float orbitRadius = shootRange * ORBIT_RADIUS_FACTOR * (0.85f + ai.orbitBias * 0.4f);
            const float angle       = EntityNoise(entity, 11) * glm::two_pi<float>();
            const auto  tangent     = SafeNormalize(lateral * flank + orbitUp * 0.25f);
            aimPoint = targetTransform.position + tangent * orbitRadius;
            aimPoint += (lateral * std::cos(angle) + orbitUp * std::sin(angle)) * (APPROACH_SPREAD * 0.25f);
            break;
        }
        case CombatManeuver::Strafe: {
            const auto side = lateral * flank * (shootRange * 0.55f);
            aimPoint += side + orbitUp * (EntityNoise(entity, 41) * 2.0f - 1.0f) * 20.0f;
            break;
        }
        case CombatManeuver::Reposition: {
            const auto away = SafeNormalize(-toTarget + lateral * flank * 1.4f + orbitUp * 0.35f, -toTarget);
            aimPoint        = transform.position + away * (shootRange * 1.2f);
            break;
        }
        case CombatManeuver::Pursue:
        default: {
            const float spread =
                APPROACH_SPREAD * (0.35f + 0.65f * EntityNoise(entity, 3)) * (ai.squadLeader != 0 ? 1.35f : 1.0f) *
                glm::clamp(distance / shootRange, 0.2f, 1.0f);
            const float angle = EntityNoise(entity, 11) * glm::two_pi<float>();
            aimPoint += (lateral * flank * std::cos(angle) + orbitUp * std::sin(angle)) * spread;
            break;
        }
    }

    return SafeNormalize(aimPoint - transform.position, toTarget);
}

void AIControlSystem::ApplySteering(SpaceShipControlComponent& spaceShipControl, const TransformComponent& transform,
                                    const glm::vec3& desiredDirection, const glm::vec3& faceDirection)
{
    const auto desired = SafeNormalize(desiredDirection, transform.GetForwardDirection());
    const auto face    = SafeNormalize(faceDirection, desired);

    // Blend movement intent with facing so orbit/strafe can still line up shots.
    const auto steerTarget = SafeNormalize(desired * 0.55f + face * 0.45f, desired);

    glm::vec3   worldTorque = glm::cross(steerTarget, transform.GetForwardDirection());
    const float forwardDot  = glm::dot(transform.GetForwardDirection(), steerTarget);

    if (glm::length(worldTorque) < 0.001f)
    {
        if (forwardDot < 0.0f)
            worldTorque = transform.GetUpDirection();
        else
            worldTorque = glm::vec3(0.0f);
    }

    spaceShipControl.pitchTorque = glm::dot(worldTorque, transform.GetRightDirection());
    spaceShipControl.yawTorque   = glm::dot(worldTorque, transform.GetUpDirection());
    spaceShipControl.rollTorque  = glm::dot(transform.GetRightDirection(), WORLD_UP);
}

void AIControlSystem::UpdateBoost(AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
                                  float alignment, float distance, float deltaTime)
{
    if (aiControlled.boostTime > 0.0f)
    {
        aiControlled.boostTime -= deltaTime;
        spaceShipControl.boostFactor = BoostFactor;
        if (aiControlled.boostTime <= 0.0f)
            aiControlled.boostCooldown = BOOST_COOLDOWN * (1.1f - aiControlled.aggression * 0.3f);
        return;
    }

    if (aiControlled.boostCooldown > 0.0f)
    {
        aiControlled.boostCooldown -= deltaTime;
        return;
    }

    const bool engageBoost =
        (distance > BOOST_ENGAGE_DISTANCE && alignment > 0.8f) || aiControlled.maneuver == CombatManeuver::Reposition;

    if (engageBoost)
    {
        aiControlled.boostTime       = BOOST_DURATION * aiControlled.boostDurationScale;
        spaceShipControl.boostFactor = BoostFactor;
    }
}
