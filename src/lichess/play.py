import os
from lichess.client import stream_game_events, make_move

GAME_FILE_DIR = "~/Documents/stase_lichess"
STASE_EXEC_DIR = "~/CLionProjects/stase2"


def play_game(token: str, game_id: str, as_white: bool):

    if as_white:
        move: str = _get_move(game_id, "")
        print(f"Playing move {move}")
        make_move(token, game_id, move)

    for event in stream_game_events(token, game_id):
        if event["type"] == "gameState":
            moves_played: str = event["moves"].strip()
            half_move_count = moves_played.count(' ') + 1

            if as_white and half_move_count % 2 == 0:
                move: str = _get_move(game_id, moves_played)
                print(f"Playing move {move}")
                make_move(token, game_id, move)
        else:
            print(f"Received event of type {event['type']}")


def _get_move(game_id: str, moves_played: str) -> str:

    with open(f"{GAME_FILE_DIR}/{game_id}.game") as file:
        file.write(moves_played)

    os.system(f"cd {STASE_EXEC_DIR}")

    os.system(f"./stase -g {game_id}")

    with open(f"{GAME_FILE_DIR}/{game_id}.game") as file:
        move: str = file.readline().strip()

    return move
