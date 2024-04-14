#include "SpaceApp.hpp"

#include <Builders/ApplicationBuilder.hpp>

int main(int argc, char const *argv[])
{
    auto app = fra::ApplicationBuilder()
                .Build<SpaceApp>();

    app->Run();

    return 0;
}
