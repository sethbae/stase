import multiprocessing
import subprocess
import sys
import os
import time
from typing import List, Any, Dict

from client import (
    respond_to_challenge,
    stream_incoming_events,
    make_move,
)
from play import play_game
from info import read_access_token, ROOT_DIR, ENV_FILE, InvalidTokenException


def rebuild_stase(pr: bool = True) -> None:
    """
    Pulls the latest version from master and builds the C++ executable.
    """
    if pr: print("Pulling changes...", end="")
    subprocess.Popen("git pull".split(), cwd=ROOT_DIR, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).wait()
    if pr: print("done")

    if pr: print("Building engine...", end="")
    subprocess.Popen("cmake .".split(), cwd=ROOT_DIR, stdout=subprocess.DEVNULL).wait()
    subprocess.Popen("make stase".split(), cwd=ROOT_DIR, stdout=subprocess.DEVNULL).wait()
    if pr: print("done")


def configure_dirs(pr: bool = True) -> None:
    if pr: print("Checking dirs...", end="")

    if not os.path.isdir("deployment"):
        os.mkdir("deployment")
    if not os.path.isdir("deployment/game_files"):
        os.mkdir("deployment/game_files")
    if not os.path.exists(ENV_FILE):
        with open(ENV_FILE, "w") as file:
            file.close()

    if pr: print("done")


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


def play_single_games():
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
            play_game(tk, game_id)
        elif event["type"] == "gameFinish":
            # no action required
            pass
        else:
            print(f"Ignoring event of type: {event['type']}")


class ChallengePool:

    def __init__(self, size: int, access_token: str):
        self.size: int = size
        self.access_token: str = access_token
        self.procs: List = [None for _ in range(self.size)]
        self.proc_names: Dict[Any, str] = {}
        self.q: List = []

    def games_in_play(self) -> int:
        return len([p for p in self.procs if p is not None])

    def add_challenge(self, event) -> None:
        self.q.append(event)

    def refresh_challenges(self) -> None:

        # get rid of dead processes
        for i in range(len(self.procs)):
            proc = self.procs[i]
            if proc is not None and not proc.is_alive():
                print(f"Game {self.proc_names[proc]} has finished")
                self.proc_names[proc] = ""
                self.procs[i] = None

        # kick off as many challenges as possible
        while len(self.q) > 0 and self.games_in_play() < self.size:
            challenge = self.q.pop(0)
            respond_to_challenge(self.access_token, challenge["challenge"]["id"])
            time.sleep(0.5)
            i: int = self.procs.index(None)
            self.procs[i] =\
                multiprocessing.Process(target=play_game, args=(self.access_token, challenge["challenge"]["id"]))
            self.procs[i].start()
            self.proc_names[self.procs[i]] = challenge["challenge"]["id"]


def play_games(token: str, max_concurrent: int) -> None:
    """
    Accepts all challenges. Plays up to :param max_concurrent: games at once, queueing
    other challenges and accepting in order received.
    """

    cpool: ChallengePool = ChallengePool(max_concurrent, token)

    print("Waiting for incoming challenges.")

    for event in stream_incoming_events(token):
        if "error" in event:
            print(f"Received error: {event}")
        elif event["type"] == "challenge":
            cpool.add_challenge(event)
        elif event["type"] == "gameStart":
            # handled inside play_game
            pass
        elif event["type"] == "gameFinish":
            # no action required
            pass
        else:
            print(f"Ignoring event of type: {event['type']}")

        cpool.refresh_challenges()


def main():

    rebuild_stase()
    configure_dirs()

    try:
        tk: str = read_access_token()
    except InvalidTokenException:
        print("Could not read access token: please place in a file called \"env\" in the root directory.")
        return

    play_games(tk, 5)


if __name__ == "__main__":
    main()
    sys.exit(0)
