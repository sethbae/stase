import sys
from src.lichess.main import run_driver

if __name__ == "__main__":
    try:
        run_driver()
    except KeyboardInterrupt:
        pass
    sys.exit(0)
