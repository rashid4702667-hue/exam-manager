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
    Copy-Item "$QtBinPath\Qt6PrintSupport.dll" .
    Copy-Item "$QtBinPath\Qt6Charts.dll" .
    Copy-Item "$QtBinPath\Qt6OpenGL.dll" .
    Copy-Item "$QtBinPath\Qt6OpenGLWidgets.dll" .
    Copy-Item "$QtBinPath\Qt6Network.dll" .
    Copy-Item "$QtBinPath\Qt6Svg.dll" .
    
    # Copy MinGW runtime DLLs
    Write-Host "Copying MinGW runtime DLLs..." -ForegroundColor Yellow
    Copy-Item "$MinGWPath\libgcc_s_seh-1.dll" .
    Copy-Item "$MinGWPath\libstdc++-6.dll" .
    Copy-Item "$MinGWPath\libwinpthread-1.dll" .
    
    # Copy platform plugins
    Write-Host "Copying platform plugins..." -ForegroundColor Yellow
    $platformsDir = "platforms"
    if (!(Test-Path $platformsDir)) { mkdir $platformsDir }
    Copy-Item "$QtPath\plugins\platforms\*" $platformsDir -Recurse -Force
    
    # Copy other plugin directories
    $pluginDirs = @("iconengines", "imageformats", "styles", "tls", "networkinformation")
    foreach ($dir in $pluginDirs) {
        if (!(Test-Path $dir)) { mkdir $dir }
        if (Test-Path "$QtPath\plugins\$dir") {
            Copy-Item "$QtPath\plugins\$dir\*" $dir -Recurse -Force
        }
    }
    
    Write-Host "Setup complete! You can now run: .\TimetablePlannerGUI.exe" -ForegroundColor Green
} else {
    Write-Host "Build failed!" -ForegroundColor Red
}