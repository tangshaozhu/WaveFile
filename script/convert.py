import os, sys

PROGRAM_PATH = r"convert.exe"

if __name__ == "__main__":
    path = sys.argv[1]
    for root, dirs, files in os.walk(path):
        for file in files:
            filepath = os.path.join(root, file)
            if filepath.endswith('.wav'):
                print(f"start converting {filepath}...")
                print(f"\"{PROGRAM_PATH}\" \"{filepath}\"")
                ret = os.popen(f"\"{PROGRAM_PATH}\" \"{filepath}\"")
                ret.read()