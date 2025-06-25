import os

include_dir = '.'
src_dir = '../src'
output_file = '../project_dump.txt'

included_cpp_files = set()

with open(output_file, 'w', encoding='utf-8') as outfile:
    # Process all .hpp files
    for hpp_filename in sorted(f for f in os.listdir(include_dir) if f.endswith('.hpp')):
        base_name = os.path.splitext(hpp_filename)[0]
        cpp_filename = base_name + '.cpp'
        hpp_path = os.path.join(include_dir, hpp_filename)
        cpp_path = os.path.join(src_dir, cpp_filename)

        # Write the .hpp file content
        outfile.write(f"\n// ===== Begin {hpp_filename} =====\n")
        with open(hpp_path, 'r', encoding='utf-8') as hpp_file:
            outfile.write(hpp_file.read())
        outfile.write(f"\n// ===== End {hpp_filename} =====\n")

        # If corresponding .cpp exists, include it immediately after
        if os.path.exists(cpp_path):
            outfile.write(f"\n// ===== Begin {cpp_filename} =====\n")
            with open(cpp_path, 'r', encoding='utf-8') as cpp_file:
                outfile.write(cpp_file.read())
            outfile.write(f"\n// ===== End {cpp_filename} =====\n")
            included_cpp_files.add(cpp_filename)

    # Include any remaining .cpp files in /src not already added
    for cpp_filename in sorted(f for f in os.listdir(src_dir) if f.endswith('.cpp')):
        if cpp_filename not in included_cpp_files:
            cpp_path = os.path.join(src_dir, cpp_filename)
            outfile.write(f"\n// ===== Begin {cpp_filename} =====\n")
            with open(cpp_path, 'r', encoding='utf-8') as cpp_file:
                outfile.write(cpp_file.read())
            outfile.write(f"\n// ===== End {cpp_filename} =====\n")

print(f"✅ Combined output written to {output_file}")
