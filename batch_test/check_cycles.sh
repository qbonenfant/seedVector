echo "Cycle detection" > cycle_simple_log.txt
for file in $(ls $1);
do
    echo "$file" >> cycle_simple_log.txt
    ~/Documents/Code/seedVector/seedVector "$1/$file" >> cycle_simple_log.txt
done;
