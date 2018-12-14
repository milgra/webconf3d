fswatch -0 -recursive -e ".*" -i "\\.c$" /Users/milgra/Desktop/Projects/kinetic/ | xargs -0 -n 1 -I % /Users/milgra/Desktop/Projects/kinetic/wrappers/makeheader %
