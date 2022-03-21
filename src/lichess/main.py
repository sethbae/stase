import multiprocessing
import subprocess
import sys
import time
from typing import List, Any, Dict

from client import (
    respond_to_challenge,
    stream_incoming_events,
    make_move,
)
from play import play_game
from info import (
    read_access_token,
    STASE_SRC_DIR,
)


def rebuild_stase() -> None:
    subprocess.Popen("git pull".split(), cwd=STASE_SRC_DIR, stdout=subprocess.PIPE).wait()
    subprocess.Popen("cmake .".split(), cwd=STASE_SRC_DIR, stdout=subprocess.PIPE).wait()
    subprocess.Popen("make stase".split(), cwd=STASE_SRC_DIR, stdout=subprocess.PIPE).wait()


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


def play_games(max_concurrent: int) -> None:
    """
    Accepts all challenges. Plays up to :param max_concurrent: games at once, queueing
    other challenges and accepting in order received.
    """

    token: str = read_access_token()
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
    print("Building engine...", end="")
    rebuild_stase()
    print("done!")
    play_games(5)


if __name__ == "__main__":
    main()
    sys.exit(0)
