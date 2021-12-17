import subprocess

from lichess.client import stream_game_events, make_move, resign_game

GAME_FILE_DIR = "/home/seth/Documents/stase_lichess"
STASE_EXEC_DIR = "/home/seth/CLionProjects/stase2"
ENGINE_USERNAME = "queen_stase_approx"


def play_game(token: str, game_id: str, time: int = 10, timeout: int = 15):

    def play_a_move(game_so_far: str) -> bool:
        move: str = _get_move(game_id, game_so_far, time=time, timeout=timeout)
        if move == "ERROR":
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
                    print("Error encountered")
                    resign_game(token, game_id)
                    return

        else:
            print(f"Received event of type {event['type']}")


def _get_move(game_id: str, moves_played: str, time: int = 10, timeout: int = 15) -> str:

    with open(f"{GAME_FILE_DIR}/{game_id}.game", "w") as file:
        file.write(moves_played)

    print("Fetching move...", end="")

    exec_stase_command: str = f"./stase -t {time} -g {game_id}"
    engine_process = subprocess.Popen(
        exec_stase_command.split(),
        cwd=STASE_EXEC_DIR,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    try:
        engine_process.wait(timeout=timeout)
        engine_failed = False
    except subprocess.TimeoutExpired:
        engine_failed = True

    output, error = engine_process.communicate()

    print("done")

    if output or error or engine_failed:
        print("Received error from engine:")
        print(f"Output: {output}")
        print(f"Error: {error}")
        return "ERROR"

    with open(f"{GAME_FILE_DIR}/{game_id}.game", "r") as file:
        move: str = file.readline().strip()

    return move
