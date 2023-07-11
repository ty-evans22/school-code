package environment;

import agent.Action;
import search.framework.Node;
import util.datastructure.XYLocation;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Useful functions for solving EightPuzzle problems.
 *
 * @author Ruediger Lunde
 * @author Tyler Evans
 */
public class EightPuzzleFunctions {

    public static final EightPuzzleBoard GOAL_STATE = new EightPuzzleBoard(new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 0 });

    public static List<Action> getActions(EightPuzzleBoard state) {
        return Stream.of(EightPuzzleBoard.UP, EightPuzzleBoard.DOWN, EightPuzzleBoard.LEFT, EightPuzzleBoard.RIGHT).
                filter(state::canMoveGap).collect(Collectors.toList());
    }

    public static EightPuzzleBoard getResult(EightPuzzleBoard state, Action action) {
        EightPuzzleBoard result = state.clone();

        if (result.canMoveGap(action)) {
            if (action == EightPuzzleBoard.UP)
                result.moveGapUp();
            else if (action == EightPuzzleBoard.DOWN)
                result.moveGapDown();
            else if (action == EightPuzzleBoard.LEFT)
                result.moveGapLeft();
            else if (action == EightPuzzleBoard.RIGHT)
                result.moveGapRight();
        }
        return result;
    }

    public static double getManhattanDistance(Node<EightPuzzleBoard, Action> node) {
        EightPuzzleBoard currState = node.getState();
        int result = 0;
        for (int val = 1; val <= 8; val++) {
            XYLocation locCurr = currState.getLocationOf(val);
            XYLocation locGoal = GOAL_STATE.getLocationOf(val);
            result += Math.abs(locGoal.getX() - locCurr.getX());
            result += Math.abs(locGoal.getY() - locCurr.getY());
        }
        return result;
    }

    public static int getNumberOfMisplacedTiles(Node<EightPuzzleBoard, Action> node) {
        EightPuzzleBoard currState = node.getState();
        int result = 0;
        for (int val = 1; val <= 8; val++)
            if (!(currState.getLocationOf(val).equals(GOAL_STATE.getLocationOf(val))))
                result++;
        return result;
    }

    public static int getNumberOfTileReversals(Node<EightPuzzleBoard, Action> node) {
        EightPuzzleBoard currState = node.getState();
        int result = 0;
        for (int val = 1; val <= 8; val++) {
            if (!(currState.getLocationOf(val).equals(GOAL_STATE.getLocationOf(val)))) {
                // check each adjacent tile to see if it is a direct reversal
                for (XYLocation loc : adjacentTiles(currState, currState.getLocationOf(val))) {
                    if (GOAL_STATE.getValueAt(loc) == val && GOAL_STATE.getValueAt(currState.getLocationOf(val)) ==
                            currState.getValueAt(loc)) {
                        result++;
                        break;
                    }
                }
            }
        }
        return result + getNumberOfMisplacedTiles(node);
    }

    public static List<XYLocation> adjacentTiles(EightPuzzleBoard currState, XYLocation loc) {
        List<XYLocation> result = new ArrayList<>();
        switch(loc.getX()) {
            case 0:
                result.add(loc.right());
                break;
            case 1:
                result.add(loc.left());
                result.add(loc.right());
                break;
            case 2:
                result.add(loc.left());
                break;
        }
        switch(loc.getY()) {
            case 0:
                result.add(loc.down());
                break;
            case 1:
                result.add(loc.up());
                result.add(loc.down());
                break;
            case 2:
                result.add(loc.up());
                break;
        }
        return result;
    }

    public static boolean isSolvable(EightPuzzleBoard puzzleBoard) {
        // count total number of inversion pairs
        // solvable if even, otherwise not
        int inversionCount = 0;
        int[] state = puzzleBoard.getState();
        for (int i = 0; i < 9; i++) {
            int curr = state[i];
            if (curr == 0)
                continue;
            for (int j = i + 1; j < 9; j++) {
                if (state[j] < curr && state[j] != 0)
                    inversionCount++;
            }
        }
        return inversionCount % 2 == 0 ? true : false;
    }

    public static boolean testGoal(EightPuzzleBoard state) {
        return state.equals(GOAL_STATE);
    }
}
