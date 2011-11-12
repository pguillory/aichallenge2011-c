#!/usr/bin/env sh
turns=$1
shift

cd `dirname $0`
# make
cd ../tools
# --player_seed 42 
# --map_file maps/test.map \
nice -n 15 ./playgame.py --end_wait=0.25 --verbose --log_dir game_logs --turns $turns --turntime 20000 \
 --fill \
 --map_file maps/maze/maze_5.map \
 "$@" \
 "python sample_bots/python/LeftyBot.py" \
 "../c/MyBot"

 # "python sample_bots/python/HunterBot.py" \
 # "python sample_bots/python/LeftyBot.py" \
 # "python sample_bots/python/GreedyBot.py"
 # --map_file maps/symmetric_random_walk/random_walk_01.map \
