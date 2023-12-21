Import("env")
import shutil
import os
import os.path

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

def post_program_action(source, target, env):
    print("Program has been built!", source, target)
    firmware_source = os.path.join(env.subst("$BUILD_DIR"), "firmware.bin")
    target_path = os.path.join(env["PROJECT_DIR"], "bin", "firmware.bin")
    print("Program path", firmware_source)
    print("Target path", target_path)
    print()
    #print(env.Dump())
    # Use case: sign a firmware, do any manipulations with ELF, etc
    # env.Execute(f"sign --elf {program_path}")
    if os.path.exists(firmware_source):
        shutil.copy(firmware_source, target_path)

#env.AddPostAction("$PROGPATH", post_program_action)
env.AddPostAction("buildprog", post_program_action)