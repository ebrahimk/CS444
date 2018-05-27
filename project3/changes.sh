#!/bin/bash
# Find remote URL for hashes (designed for GitHub-hosted projects)
origin=`git config remote.origin.url`
base=`dirname "$origin"`/`basename "$origin" .git`

# Output LaTeX table in chronological order
echo "\\begin{tabular}{l l l}\\textbf{Date} & \\textbf{Author} & \\textbf{Description}\\\\\\hline"
git log --pretty=format:"%ad & %an & %s\\\\\\hline" --reverse
echo "\end{tabular}"
