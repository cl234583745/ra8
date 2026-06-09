# Configuration file for custom user settings

# Disable RASC automatic regeneration in CMake builds
# Set to echo to skip RASC regeneration (RASC should be run manually via IDE if needed)
set(RASC_EXE_PATH "echo")
message(NOTICE "RASC automatic regeneration disabled - using 'echo' for RASC_EXE_PATH")
message(NOTICE "Generated content already exists in ra_gen/ directory from IDE or manual RASC generation")
