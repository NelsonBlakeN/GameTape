# Description

EA College Football 25 has a Dynasty mode, where a player can be the head coach of a NCAA D1 FBS school. Players can play games, manage their team, and scout/recruit new athletes. 

During the recruiting process, a player can view recruits from a complete list and search by factors like position or location. The complete list shows limited information such as position, play style, location, star value, and physical traits like height and weight. The player can then choose individual recruits to track and scout for their next incoming class. The in game tracking system only allows 35 recruits at one time.

This tracking limit can pose some problems over the course of a recruiting cycle. If a player has reached their tracking limit but wants to look for more recruits for a position they still need, they would have to remove a recruit from somewhere in their list. However, those recuits could still be worth tracking if they were valuable as fallback options in case the player was not able to secure a committment from a more desireable recruit. Similarly, if a player wants to look for new recruits to scout from the complete list, there is minimal information to tell the player which recruits have already been scouted. This can cause the player to select recruits they have already removed and forgotten, and force them into a multi step process of removing the player, navigating back through a menu that does not remember it's last filter state, and look for more.

This project aims to help solve these issues using OCR and game streaming. By streaming the game to a computer, this software can run on the same machine and observe the list of recruits while the player navigates through the menu. The program then aggregates the relevant information, converts it to text using OCR, and saves it to a CSV file. The player can then use this information to track which recruits have been scouted already or are worth revisiting if necessary.

With this tool, players can make more informed decisions, efficiently track potential recruits, and avoid redundant scouting efforts.
