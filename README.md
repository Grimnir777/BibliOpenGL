# Projet de graphisme et visualisation - BibliOpenGL
## Installation du projet

```sh
git clone https://github.com/Grimnir777/BibliOpenGL
```

## Configuration
Ouvrir le projet grâce au fichier .sln
Puis se rendre sur les propriétés du projet :
- Dans Répertoire VC++ :
  - Renseigner  le dossier ./depandancies/include en tant que répertoire Include
  - Renseigner  le dossier ./depandancies/include/freetype/include en tant que répertoire Include
  - Renseigner  le dossier ./depandancies/librairies en tant que répertoire de  bibliothèques

- Dans C/C++ :
  - Renseigner le dossier ./depandancies/include/glm en tant qu'autre répertoire include

- Dans L'éditeur de liens / Général :
  - Renseigner le dossier ./depandancies/include/freetype/win32 en tant que répertoire de bibliothèques supplémentaires
 
- Dans L'éditeur de liens / entrée :
  - Rajouter en tant que dépendances supplémentaires:
    - opengl32.lib
    - glfw3.lib
    - freetype.lib

