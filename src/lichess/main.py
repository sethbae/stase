import subprocess
import sys
from client import (
    read_access_token,
    get_incoming_challenges,
    respond_to_challenge,
    stream_incoming_events,
    make_move,
    register_account_as_bot
)
from play import play_game


def rebuild_stase() -> None:
    subprocess.Popen("cmake .".split(), cwd="/home/seth/CLionProjects/stase2", stdout=subprocess.PIPE).wait()
    subprocess.Popen("make stase".split(), cwd="/home/seth/CLionProjects/stase2", stdout=subprocess.PIPE).wait()


def repl_game():
    tk: str = read_access_token()

    for event in stream_incoming_events(tk):

        if event["type"] == "challenge":
            challenge_id = event["challenge"]["id"]
            respond_to_challenge(tk, challenge_id)

        if event["type"] == "gameStart":
            game_id = event["game"]["id"]

            while True:
                uci_in = input(">> ")

                res = make_move(tk, game_id, uci_in)

                if not res:
                    print("Move failed!")


def play_single_games(time: int = 10, timeout: int = 15):
    """
    Accepts all challenges but only plays one game at a time.
    """
    tk: str = read_access_token()

    print("Waiting for incoming challenges.")

    for event in stream_incoming_events(tk):
        if "error" in event:
            print(f"Received error: {event}")
        elif event["type"] == "challenge":
            challenge_id = event["challenge"]["id"]
            challenger_name = event["challenge"]["challenger"]["name"]
            respond_to_challenge(tk, challenge_id)
            print(f"Accepted challenge from {challenger_name}")
        elif event["type"] == "gameStart":
            print(f"Starting game {event['game']['id']}")
            game_id = event["game"]["id"]
            play_game(tk, game_id, time=time, timeout=timeout)
        else:
            print(f"Ignoring event of type: {event['type']}")


def main():
    print("Building engine...", end="")
    rebuild_stase()
    print("done!")
    play_single_games(time=5, timeout=10)


if __name__ == "__main__":
    main()
    sys.exit(0)
