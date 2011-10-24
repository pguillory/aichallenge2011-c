#!/usr/bin/env sh
cd `dirname $0`
make
cd ../tools
# --player_seed 42 
# --map_file maps/test.map \
./playglobals.py --end_wait=0.25 --verbose --log_dir game_logs --turns 75 \
 --map_file maps/test.map \
 "$@" \
 "../c/MyBot" \
 "python sample_bots/python/LeftyBot.py" \
 "python sample_bots/python/HunterBot.py" \
 "python sample_bots/python/GreedyBot.py"
