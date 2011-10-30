#!/usr/bin/env sh
cd `dirname $0`
# make
cd ../tools
# --player_seed 42 
# --map_file maps/test.map \
nice 15 ./playgame.py --end_wait=0.25 --verbose --log_dir game_logs --turns 500 \
 --fill \
 --map_file maps/symmetric_random_walk/random_walk_01.map \
 "$@" \
 "../c/MyBot"

 # "python sample_bots/python/HunterBot.py" \
 # "python sample_bots/python/LeftyBot.py" \
 # "python sample_bots/python/GreedyBot.py"
