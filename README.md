# Ne Zha 3D Graphics Application

## Course Information
**Tunku Abdul Rahman University of Technology and Management (TARUMT)**  
**BMCS2173 Graphics Programming**  
**Group Assignment (60% Coursework Assessment)**  

**Learning Outcome:**  
CLO3 – Produce a 3D interactive graphics application using C++ and OpenGL.

---

## Project Overview
This project is a 3D interactive character prototype inspired by *Ne Zha*.  
The application demonstrates core graphics programming concepts using **C++ and OpenGL**, including modeling, transformations, lighting, interaction, and animation.

---

## Features

### 1. 3D Character Model
- Custom-designed Ne Zha character built using OpenGL primitives
- Structured using hierarchical modeling

### 2. Camera / Viewport
- Interactive camera movement
- Users can view the model from different angles

### 3. Interactive Controls
- Manipulation of character parts (arms, legs, etc.)
- Keyboard-based interaction

### 4. Animation
- Basic animation applied to character actions
- User-triggered animation sequences

### 5. Customization
- Ability to modify:
  - Colors
  - Shape / scale (where applicable)

---

## System Requirements
- **IDE:** Visual Studio 2022
- **Language:** C++
- **Graphics API:** OpenGL (GLU / GLUT)

---

## Project Structure
```
NeZha/
│
├── Main.cpp
├── NeZha.sln
├── NeZha.vcxproj
├── Assets/ ← (NOT included in repo)
└── README.md
```
---

## ⚠️ Assets Setup (IMPORTANT)

The `Assets/` folder is **excluded from this repository** due to large file sizes.

### Download Assets:
https://drive.google.com/drive/folders/1Bq0s4iyMsdx_GKPOkWPj5xPBYJk9tpkS?usp=drive_link

### Setup Instructions:
1. Download all files from the link above  
2. Place the `Assets` folder in the **project root directory**  
   (same level as `NeZha.sln`)

Correct structure:
```
NeZha/
├── Assets/
│ ├── Environment/
│ ├── Character/
│ └── ...
├── NeZha.sln
```

> If the folder structure is incorrect, textures will fail to load.

---

## Additional Resources

### Report
https://docs.google.com/document/d/1OwU_G4HabOtHyup5A00Ly1GxMyS-LhErPepVSthXlU4/edit?usp=drive_link

### Character Design (Figma)
https://www.figma.com/design/Aom032BJPb2yL71bMHnOt6/Ne-Zha-Design?node-id=0-1&p=f&t=CLHgOhgAz4wobYtR-0

### Other Resources
https://drive.google.com/drive/u/5/folders/1uf4sTl_5yqPHZ8PLtyYA64hkF_ly64kR

---

## Notes on Assets Management
- The repository **intentionally excludes the `Assets/` folder**
- Always refer to the Google Drive link for the **latest textures**
- Do **NOT** commit assets when pushing to GitHub

---

## Build & Run
1. Open `NeZha.sln` in Visual Studio 2022  
2. Ensure `Assets/` folder is correctly placed  
3. Build and run the project  

---

## Disclaimer
- "Ne Zha" and related characters are copyrighted by GMC Pictures  
- This project is for **educational purposes only**

---