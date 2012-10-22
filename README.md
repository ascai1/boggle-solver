Boggle solver in C. Compile with gcc and run.

Usage:
	boggle-solver [boggle layout] [word list]

Boggle layout format:
- First line contains two whitespace-delimited integers, indicating the height and width of the Boggle grid respectively.
- "H" lines follow, where H = the height of the grid, as indicated in line one. Each line contains "W" alphabetical characters, where W = the width of the grid, as indicated in line one. The first of these lines corresponds to the topmost row of the Boggle grid, the first character on this line corresponds to the letter at the top-left corner of the grid, etc.

Word list is limited to words of max length 126.

Sample word list taken from https://github.com/potatoqueue/BoggleSolver.
