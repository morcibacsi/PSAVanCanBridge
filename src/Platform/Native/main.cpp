#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>

#include "Application/BridgeRuntime.hpp"
#include "Helpers/ConfigFile.hpp"
#include "Platform/Native/FileReplayTransport.hpp"
#include "Platform/Native/SimulatedClock.hpp"
#include "Protocol/AEE2001/AEE2001ComfortBus.hpp"
#include "Protocol/AEE2004/AEE2004ComfortBus.hpp"

namespace
{
    struct Options
    {
        std::string input;
        std::string output;
        std::string settings;
        uint64_t runAfterInputMs = 1000;
        bool realtime = false;
        double speed = 1.0;
    };

    void PrintUsage(const char* executable)
    {
        std::printf(
            "Usage: %s --input <capture.txt> [--output <transmitted.txt>]\n"
            "          [--settings <settings.json>]\n"
            "          [--realtime] [--speed <factor>] [--run-after-input-ms <ms>]\n",
            executable);
    }

    bool ParseOptions(int argc, char** argv, Options& options)
    {
        for (int i = 1; i < argc; ++i)
        {
            const std::string argument = argv[i];
            if ((argument == "--input" || argument == "--output" || argument == "--settings" || argument == "--speed"
                || argument == "--run-after-input-ms") && i + 1 >= argc)
            {
                return false;
            }
            if (argument == "--input") options.input = argv[++i];
            else if (argument == "--output") options.output = argv[++i];
            else if (argument == "--settings") options.settings = argv[++i];
            else if (argument == "--realtime") options.realtime = true;
            else if (argument == "--speed") options.speed = std::strtod(argv[++i], nullptr);
            else if (argument == "--run-after-input-ms") options.runAfterInputMs = std::strtoull(argv[++i], nullptr, 10);
            else if (argument == "--help" || argument == "-h") return false;
            else if (options.input.empty() && argument.rfind("--", 0) != 0) options.input = argument;
            else return false;
        }
        return !options.input.empty() && options.speed > 0.0;
    }
}

int main(int argc, char** argv)
{
    Options options;
    if (!ParseOptions(argc, argv, options))
    {
        PrintUsage(argv[0]);
        return 2;
    }

    SimulatedClock clock;
    CarState carState{};
    carState.SOURCE_PROTOCOL = static_cast<uint8_t>(ProtocolType::AEE2001);
    carState.DESTINATION_PROTOCOL = static_cast<uint8_t>(ProtocolType::AEE2004);

    FileReplayTransport sourceTransport("file-backed VAN", ProtocolType::AEE2001, &clock);
    FileReplayTransport destinationTransport("captured CAN", ProtocolType::AEE2004, &clock);
    std::string error;
    if (!sourceTransport.LoadCapture(options.input, error))
    {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    if (!options.output.empty() && !destinationTransport.OpenOutput(options.output, error))
    {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    const char* settingsFile = options.settings.empty()
        ? ".pio/build/native/settings.json"
        : options.settings.c_str();
    ConfigFile configStore(&carState, settingsFile);
    if (!options.settings.empty() && !configStore.Read())
    {
        std::fprintf(stderr, "Failed to read native settings from %s\n", settingsFile);
        return 1;
    }
    MessageScheduler sourceScheduler;
    MessageScheduler destinationScheduler;
    AEE2001ComfortBus sourceProtocol(&carState, &sourceTransport, &sourceScheduler);
    AEE2004ComfortBus destinationProtocol(
        &carState, &destinationTransport, &destinationScheduler, &configStore);
    BridgeRuntime application(
        &carState, &clock, &sourceProtocol, &destinationProtocol, nullptr, BridgeRuntime::Hooks{});
    if (!application.Initialize())
    {
        std::fputs("Failed to initialize bridge runtime\n", stderr);
        return 1;
    }

    std::puts("Native bridge started: AEE2001 file replay -> AEE2004 capture");
    uint64_t previousTime = 0;
    while (sourceTransport.HasPendingInput())
    {
        const uint64_t nextTime = sourceTransport.NextInputTimeMs();
        if (options.realtime && nextTime > previousTime)
        {
            const auto delay = static_cast<uint64_t>((nextTime - previousTime) / options.speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        clock.SetTimeMs(nextTime);
        application.ProcessOnce();
        previousTime = nextTime;
    }

    const uint64_t finishTime = clock.NowMs() + options.runAfterInputMs;
    while (clock.NowMs() < finishTime)
    {
        const uint64_t remaining = finishTime - clock.NowMs();
        clock.AdvanceTimeMs(remaining < 10 ? remaining : 10);
        if (options.realtime)
        {
            const auto delay = static_cast<uint64_t>(10.0 / options.speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        application.ProcessOnce();
    }

    std::printf("Replay complete at %llu ms; %zu destination frames transmitted.\n",
        static_cast<unsigned long long>(clock.NowMs()),
        destinationTransport.Transmitted().size());

    //readline to prevent console from closing immediately

    std::puts("Press Enter to exit...");
    std::fflush(stdout);

    std::getchar();
    return 0;
}
