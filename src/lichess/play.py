import subprocess
import time

from client import (
    stream_game_events,
    make_move,
    resign_game
)
from info import (
    GAME_FILE_DIR,
    STASE_SRC_DIR,
    ENGINE_USERNAME
)


def play_game(token: str, game_id: str, think_time: int = 10, timeout: int = 15):

    def play_a_move(game_so_far: str) -> bool:
        move: str = _get_move(game_id, game_so_far, think_time=think_time, timeout=timeout)
        if move == "ERROR":
            print("Encountered engine error")
            return False
        else:
            print(f"Playing move {move}")
            return make_move(token, game_id, move)

    as_white: bool = False

    for event in stream_game_events(token, game_id):

        if event["type"] == "gameFull":

            print("Received full game information")

            as_white = (event["white"]["id"] == ENGINE_USERNAME)

            if as_white:
                if not play_a_move(""):
                    resign_game(token, game_id)
                    return

        elif event["type"] == "gameState":

            moves_played: str = event["moves"].strip()
            half_move_count = moves_played.count(' ') + 1

            if (as_white and half_move_count % 2 == 0)\
                    or (not as_white and half_move_count % 2 == 1):
                if not play_a_move(moves_played):
                    print("Error encountered: resigning the game")
                    resign_game(token, game_id)
                    return

        elif event["type"] == "gameFinish":

            print(f"Game {event['game']['id']} has ended")
            return

        else:
            print(f"Received event of type {event['type']}")


def _get_move(game_id: str, moves_played: str, think_time: int = 10, timeout: int = 15) -> str:

    with open(f"{GAME_FILE_DIR}/{game_id}.game", "w") as file:
        file.write(moves_played)

    print("Fetching move...", end="")

    exec_stase_command: str = f"./stase -t {think_time} -g {game_id}"
    engine_process = subprocess.Popen(
        exec_stase_command.split(),
        cwd=STASE_SRC_DIR,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    engine_failed = True
    stop_time = time.time() + timeout

    while time.time() < stop_time:
        time.sleep(0.2)
        if engine_process.poll():
            engine_failed = False
            break

    if engine_failed:
        engine_process.kill()
        return "ERROR"

    with open(f"{GAME_FILE_DIR}/{game_id}.game", "r") as file:
        move: str = file.readline().strip()

    return move
