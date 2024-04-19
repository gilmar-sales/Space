#include "SpaceApp.hpp"

#include <Builders/ApplicationBuilder.hpp>

std::shared_ptr<fra::MeshPool> gMeshPool;

int main(int argc, char const* argv[])
{
    auto app = fra::ApplicationBuilder()
                   .WithWindow([](fra::WindowBuilder& windowBuilder) {
                       windowBuilder.SetVSync(false);
                   })
                   .WithRenderer([](fra::RendererBuilder& rendererBuilder) {
                       rendererBuilder
                           .SetSamples(vk::SampleCountFlagBits::e8)
                           .SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
                   })
                   .Build<SpaceApp>();

    app->Run();

    return 0;
}
