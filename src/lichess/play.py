from src.lichess.engine_client import EngineClient
from src.lichess.client import (
    stream_game_events,
    make_move,
    resign_game,
    post_to_chat
)
from src.lichess.info import (
    ENGINE_USERNAME
)

THINK_TIME_PROPORTION: float = 0.025


def play_game(token: str, game_id: str):

    def play_a_move(think_time) -> bool:
        move: str = engine.get_move(think_time)
        if move == "ERROR":
            print("Encountered engine error")
            return False
        elif move == "no move":
            print("No move possible")
            return True
        else:
            print(f"{move}")
            return make_move(token, game_id, move)

    post_to_chat(token, game_id, "Hello there!")
    engine: EngineClient = EngineClient()

    as_white: bool = False

    for event in stream_game_events(token, game_id):

        if "error" in event:
            print(f"play game for {game_id} received error")
            return

        if event["type"] == "gameFull":

            print("Received full game information")

            as_white = (event["white"]["id"] == ENGINE_USERNAME)

            if as_white:
                if not play_a_move(think_time=5):
                    resign_game(token, game_id)
                    return

        elif event["status"] != "started":
            print(f"Game {game_id} has ended")
            return

        elif event["type"] == "gameState":

            move_played: str = event["moves"].split()[-1]
            half_move_count = event["moves"].count(' ') + 1
            millis_remaining = event["wtime"] if as_white else event["btime"]
            print(f"{move_played} played")
            print(f"{half_move_count} half moves")

            if (as_white and half_move_count % 2 == 0)\
                    or (not as_white and half_move_count % 2 == 1):
                # update the engine with the opponent's move
                engine.advance_position(move_played)
                # play a move of our own
                if not play_a_move(int((millis_remaining * THINK_TIME_PROPORTION) / 1000)):
                    print("Error encountered: resigning the game")
                    resign_game(token, game_id)
                    return
        else:
            print(f"Received event of type {event['type']}")
