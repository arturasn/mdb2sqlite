# mdb2sqlite
<p align="center"><img src="img/start-window.png"></p>
<p align="center">mdb2sqlite is a simple tool with an objective to simplify Microsoft Access database conversion to SQLite.</p>

## Features
Abillity to transfer:
* Tables
* Fields
* Field Types
  * TEXT
  * REAL
  * INTEGER
  * BLOB
* Records
* Relationships(MS Accces) / Triggers(SQLite)
* Indexes 
* Indexes properties
  * UNIQUE
  * COLLATE NO CASE
* Field comments
* Constraints
  * PRIMARY KEY
  * FOREIGN KEY
  * NOT NULL
  * AUTOINCREMENT
  * DEFAULT
  * UNIQUE
  * COLLATE NO CASE
  
Additional attributes:
 * Multiple primary key support
 * Trim text values - Trim spaces from data from the beggining and end of the record.
 * Reserved key word list - Used to generate warnings if SQLite keywords are used in transferable data thus notifying users of pottencialy uncorrect locations.
 * Foreign key both primary fields - If this element is used FOREIGN KEY is not created if both keys in the relationship are PRIMARY KEYS.
 * SQLite SQL statement dump to a text file.
 
Colourful log:
 * All transfers display
 * Warnings display
 * Errors display
  
## Screenshoots
![Alt text](img/configuration-window.png)
![Alt text](img/structure-preview-window.png)

## Compiling
To build the project yourself you have to build wxWidgets for your appropriate toolset and then generate the project using CMake:

1) Browse to mdb2sqlite/wxwidgets/build/msw. 
2) Choose the appropriate solution file for your target toolset.
3) Build your target configuration DLL Debug/DLL Release.
4) Generate mdb2sqlite project using CMake selecting the required toolset.

## License
mdb2sqlite is MIT licensed.
