Import("env")
import os
import gzip
from pathlib import Path
import mimetypes

ARDUINO_INCLUDES = "#include <Arduino.h>\n\n"
BYTES_PER_LINE = 20
INDENT = "  "
INCLUDE_PATH = "src/Helpers/WebServer/gzipped_webpage_data.h"  # Output file path
STATIC_DIR = "data"  # Directory containing static files

def get_files_recursive(directory):
    """Recursively collect all files from the specified directory."""
    files = []
    for root, _, filenames in os.walk(directory):
        for filename in filenames:
            files.append(os.path.join(root, filename))
    return files

def clean_and_open(path):
    """Clean the existing file and open it for writing."""
    if os.path.exists(path):
        os.remove(path)
    return open(path, "w+")

def generate_cpp_variable(file_path, file_id, file_buffer):
    """Generate the C++ variable for the given file."""
    variable_name = f"ESP_REACT_DATA_{file_id}"
    compressed_data = gzip.compress(file_buffer)
    hex_array = ", ".join(
        f"0x{byte:02X}" for byte in compressed_data
    )

    lines = []
    for i in range(0, len(hex_array), BYTES_PER_LINE * 6):
        lines.append(INDENT + hex_array[i:i + BYTES_PER_LINE * 6])

    cpp_array = "const uint8_t {}[] = {{\n".format(variable_name) \
                + "\n".join(lines) + "\n};\n"
    return cpp_array, variable_name, len(compressed_data)

def generate_www_class(file_info):
    """Generate the WWWData class with a registerRoutes method."""
    handler_lines = [
        '{}handler("{}", "{}", {}, {});'.format(INDENT * 3, uri, mime, var, size)
        for uri, mime, var, size in file_info
    ]

    return """
typedef std::function<void(const String& uri, const String& contentType, const uint8_t * content, size_t len)> RouteRegistrationHandler;

class WWWData {{
{0}public:
{0}static void registerRoutes(RouteRegistrationHandler handler) {{
{1}
{0}}}
}};
""".format(INDENT, "\n".join(handler_lines))

#def generate_byte_arrays_for_webpage(source, target, env):
def generate_byte_arrays_for_webpage():
    """Main function to generate the C++ file."""
    file_info = []
    output_file = clean_and_open(INCLUDE_PATH)
    try:
        # Write Arduino includes
        #output_file.write(ARDUINO_INCLUDES)

        # Process each file in the static directory
        for i, file_path in enumerate(get_files_recursive(STATIC_DIR)):
            relative_path = os.path.relpath(file_path, STATIC_DIR).replace(os.sep, "/")
            mime_type = mimetypes.guess_type(relative_path)[0] or "application/octet-stream"

            with open(file_path, "rb") as f:
                file_buffer = f.read()

            cpp_array, variable_name, size = generate_cpp_variable(file_path, i, file_buffer)
            output_file.write(cpp_array)

            file_info.append((f"/{relative_path}", mime_type, variable_name, size))

        # Write WWWData class
        #output_file.write(generate_www_class(file_info))

    finally:
        output_file.close()

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)
#env.AddPreAction("buildprog", generate_byte_arrays_for_webpage)
#env.AddPostAction("buildprog", generate_byte_arrays_for_webpage)
#print("Build dir", env.subst("${BUILD_DIR}"))
#env.AddPreAction("${BUILD_DIR}/esp32-c6/src/main.cpp.o", generate_byte_arrays_for_webpage)
generate_byte_arrays_for_webpage()
