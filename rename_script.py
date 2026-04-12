import os

files = [
    "src/main.c",
    "src/alienfx-area51-application.h",
    "src/alienfx-area51-application.c",
    "src/alienfx-area51-window.h",
    "src/alienfx-area51-window.c",
    "src/alienfx-area51.gresource.xml",
    "src/alienfx-area51-window.ui",
    "src/AlienFX-Area51.desktop",
    "README.md"
]

for filepath in files:
    if os.path.exists(filepath):
        with open(filepath, 'r') as f:
            content = f.read()
        
        # Add a derived comment in c/h files
        if filepath.endswith('.c') or filepath.endswith('.h'):
            if "Derived from AlienFX-Aurora" not in content:
                content = "/* Derived from AlienFX-Aurora */\n" + content
                
        content = content.replace("aurora", "area51")
        content = content.replace("Aurora", "Area51")
        content = content.replace("AURORA", "AREA51")
        
        # Specific UI fixes
        content = content.replace("AlienFX Area51", "AlienFX Area-51")
        
        with open(filepath, 'w') as f:
            f.write(content)
