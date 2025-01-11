#include "SpaceApp.hpp"

#include <Builders/ApplicationBuilder.hpp>

int main(int argc, char const* argv[])
{
    const auto app =
        fra::ApplicationBuilder()
            .WithWindow([](fra::WindowBuilder& windowBuilder) {
                windowBuilder.SetWidth(1920).SetHeight(1080).SetVSync(false);
            })
            .WithRenderer([](fra::RendererBuilder& rendererBuilder) {
                rendererBuilder.SetSamples(vk::SampleCountFlagBits::e8)
                    .SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f })
                    .SetDrawDistance(100'000.0f);
            })
            .Build<SpaceApp>();

    app->Run();

    return 0;
}
