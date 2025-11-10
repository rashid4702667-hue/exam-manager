# Build script for TimetablePlannerGUI
Write-Host "Setting up Qt environment..." -ForegroundColor Green

# Qt paths
$QtPath = "C:\Qt\6.10.0\mingw_64"
$QtToolsPath = "C:\Qt\Tools"
$QtBinPath = "$QtPath\bin"
$MinGWPath = "$QtToolsPath\mingw1310_64\bin" # Using MinGW 13.1.0
$CMakePath = "$QtToolsPath\CMake_64\bin"
$NinjaPath = "$QtToolsPath\Ninja"

# Remove old MinGW from PATH if present
$env:PATH = ($env:PATH.Split(';') | Where-Object { $_ -notlike '*MinGW*' }) -join ';'

# Add paths to environment (order is important)
$env:PATH = "$MinGWPath;$QtBinPath;$CMakePath;$NinjaPath;" + $env:PATH

Write-Host "Qt PATH setup complete" -ForegroundColor Green
Write-Host "Building project..." -ForegroundColor Green

# Create and enter build directory
$buildDir = "build"
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
mkdir $buildDir
cd $buildDir

# Configure with CMake
Write-Host "Running CMake configuration..." -ForegroundColor Yellow
cmake .. -G "Ninja" `
    -DCMAKE_PREFIX_PATH="$QtPath" `
    -DCMAKE_C_COMPILER="$MinGWPath\gcc.exe" `
    -DCMAKE_CXX_COMPILER="$MinGWPath\g++.exe" `
    -DCMAKE_BUILD_TYPE=Release

# Build
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build .

# Check if build was successful
if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
    
    # Copy required DLLs
    Write-Host "Copying Qt DLLs..." -ForegroundColor Yellow
    Copy-Item "$QtBinPath\Qt6Core.dll" .
    Copy-Item "$QtBinPath\Qt6Gui.dll" .
    Copy-Item "$QtBinPath\Qt6Widgets.dll" .
    
    Write-Host "Setup complete! You can now run: .\TimetablePlannerGUI.exe" -ForegroundColor Green
} else {
    Write-Host "Build failed!" -ForegroundColor Red
}