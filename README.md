# Waybersite
The waybersite project is a website with a simple http server and database support for places guesses on football matches to compete with your friends. It supports seperate entries for group and playoff stages, as well as guessing group standings. The scores are updated by an admin after each match and you can see your standings in the leaderboard.
As this site is meant to be used privately, account creation needs to be done by an admin. An admin account has to be created through the code on first use. After login, users can freely edit their own login details.

Because of the semi-modular design, it is possible to swap out the matches and teams to your liking. There are some hardcoded values and entries that need to be changed. While it is possible to use this side for other purposes, it is not recommended.

The side collects no personal data and is very leightweight. All user entries are stored in a database, making it easy to access and work on.

Some elements that this site uses, like a news section, a privacy policy and similar is not included in this repository and needs to be added afterwards.

Finally, please note that the front-end of this website is only available in German.

## Screenshots

### The Group Stage Matches
<img width="965" height="825" alt="image" src="https://github.com/user-attachments/assets/c91a3fce-a358-4805-8e8d-c9d67bbcb295" />

### The Playoff Matches
<img width="1012" height="926" alt="image" src="https://github.com/user-attachments/assets/a9ea4687-417e-4641-af46-b059c1c4837b" />

### The Group Standings
<img width="911" height="912" alt="image" src="https://github.com/user-attachments/assets/8de468a6-fa68-4d69-89bd-5eade84c4554" />

## Compiling
If you have the right libraries and tools installed, you simply need to run ```make``` as an appropriate build script is already included. A list of the required libraries can be found down below, or in the build script. Please note that the included build script requires linux to function.

## Third-Party Libs
libsodium\
SQLite\
cJSON
