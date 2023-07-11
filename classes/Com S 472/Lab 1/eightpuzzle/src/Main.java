import agent.Action;
import environment.EightPuzzleBoard;
import environment.EightPuzzleFunctions;
import search.agent.SearchAgent;
import search.framework.Metrics;
import search.framework.SearchForActions;
import search.framework.problem.GeneralProblem;
import search.framework.problem.Problem;
import search.framework.qsearch.GraphSearch;
import search.informed.AStarSearch;
import search.uninformed.BreadthFirstSearch;
import search.uninformed.IterativeDeepeningSearch;
import util.Util;

import java.io.*;
import java.util.*;
import java.lang.*;

/**
 * Issues:
 *      - Algorithm time is not accurate for subsequent algorithms if BFS or IDS
 *
 * @author Tyler Evans
 */
public class Main {

    public static void main(String[] args) {

        if (args.length != 2) {
            System.out.println("Incorrect number of arguments passed.");
            return;
        }

        List<EightPuzzleBoard> puzzleList = getPuzzlesFromFile(args[0]);

        switch(args[1]) {
            case "BFS":
                solveBFS(puzzleList);
                break;
            case "IDS":
                solveIDS(puzzleList);
                break;
            case "h1":
                solveH1(puzzleList);
                break;
            case "h2":
                solveH2(puzzleList);
                break;
            case "h3":
                solveH3(puzzleList);
                break;
            default:
                System.out.println("Invalid algorithm specified.");
        }

        System.out.println("Average Run Time: " + getTime(Util.getAvgRunTime(puzzleList.size())));
        System.out.println("Average Nodes Generated: " + Util.getAvgNodeCount(puzzleList.size()));
    }

    public static void solveBFS(List<EightPuzzleBoard> puzzleList) {
        for (EightPuzzleBoard puzzle : puzzleList) {
            if (!EightPuzzleFunctions.isSolvable(puzzle)) {
                System.out.println();
                System.out.println("The inputted puzzle is not solvable.");
                System.out.println(puzzle.toString());
                continue;
            }

            System.out.println(puzzle.toString());

            Problem<EightPuzzleBoard, Action> problem = new GeneralProblem<>(puzzle, EightPuzzleFunctions::getActions,
                    EightPuzzleFunctions::getResult, EightPuzzleFunctions::testGoal);
            SearchForActions<EightPuzzleBoard, Action> search = new BreadthFirstSearch<>(new GraphSearch<>());
            printOutput(search.getMetrics(), search.findActions(problem));
        }
    }

    public static void solveIDS(List<EightPuzzleBoard> puzzleList) {
        for (EightPuzzleBoard puzzle : puzzleList) {
            if (!EightPuzzleFunctions.isSolvable(puzzle)) {
                System.out.println();
                System.out.println("The inputted puzzle is not solvable.");
                System.out.println(puzzle.toString());
                continue;
            }

            System.out.println(puzzle.toString());

            Problem<EightPuzzleBoard, Action> problem = new GeneralProblem<>(puzzle, EightPuzzleFunctions::getActions,
                    EightPuzzleFunctions::getResult, EightPuzzleFunctions::testGoal);
//            Util.updateAlgoStartTime();
            SearchForActions<EightPuzzleBoard, Action> search =
                    new IterativeDeepeningSearch<EightPuzzleBoard, Action>().setCycleDetection(true);
//            Util.updateAlgoEndTime();
            printOutput(search.getMetrics(), search.findActions(problem));
        }
    }

    public static void solveH1(List<EightPuzzleBoard> puzzleList) {
        for (EightPuzzleBoard puzzle : puzzleList) {
            if (!EightPuzzleFunctions.isSolvable(puzzle)) {
                System.out.println();
                System.out.println("The inputted puzzle is not solvable.");
                System.out.println(puzzle.toString());
                continue;
            }

            System.out.println(puzzle.toString());

            Problem<EightPuzzleBoard, Action> problem = new GeneralProblem<>(puzzle, EightPuzzleFunctions::getActions,
                    EightPuzzleFunctions::getResult, EightPuzzleFunctions::testGoal);
            SearchForActions<EightPuzzleBoard, Action> search = new AStarSearch<>(new GraphSearch<>(),
                    EightPuzzleFunctions::getNumberOfMisplacedTiles);
            Util.updateAlgoStartTime();
            SearchAgent<Object, EightPuzzleBoard, Action> agent = new SearchAgent<>(problem, search);
            Util.updateAlgoEndTime();
            printAStarOutput(agent);
        }
    }

    public static void solveH2(List<EightPuzzleBoard> puzzleList) {
        for (EightPuzzleBoard puzzle : puzzleList) {
            if (!EightPuzzleFunctions.isSolvable(puzzle)) {
                System.out.println();
                System.out.println("The inputted puzzle is not solvable.");
                System.out.println(puzzle.toString());
                continue;
            }

            System.out.println(puzzle.toString());

            Problem<EightPuzzleBoard, Action> problem = new GeneralProblem<>(puzzle, EightPuzzleFunctions::getActions,
                    EightPuzzleFunctions::getResult, EightPuzzleFunctions::testGoal);
            SearchForActions<EightPuzzleBoard, Action> search = new AStarSearch<>(new GraphSearch<>(),
                    EightPuzzleFunctions::getManhattanDistance);
            Util.updateAlgoStartTime();
            SearchAgent<Object, EightPuzzleBoard, Action> agent = new SearchAgent<>(problem, search);
            Util.updateAlgoEndTime();
            printAStarOutput(agent);
        }
    }

    public static void solveH3(List<EightPuzzleBoard> puzzleList) {
        for (EightPuzzleBoard puzzle : puzzleList) {
            if (!EightPuzzleFunctions.isSolvable(puzzle)) {
                System.out.println();
                System.out.println("The inputted puzzle is not solvable.");
                System.out.println(puzzle.toString());
                continue;
            }

            System.out.println(puzzle.toString());

            Problem<EightPuzzleBoard, Action> problem = new GeneralProblem<>(puzzle, EightPuzzleFunctions::getActions,
                    EightPuzzleFunctions::getResult, EightPuzzleFunctions::testGoal);
            SearchForActions<EightPuzzleBoard, Action> search = new AStarSearch<>(new GraphSearch<>(),
                    EightPuzzleFunctions::getNumberOfTileReversals);
            Util.updateAlgoStartTime();
            SearchAgent<Object, EightPuzzleBoard, Action> agent = new SearchAgent<>(problem, search);
            Util.updateAlgoEndTime();
            printAStarOutput(agent);
        }
    }

    /**
     * Returns the string representation of the algorithm's execution time.
     *
     * @return the string representation of the algorithm's execution time
     */
    public static String getTime(long time) {
        StringBuilder sb = new StringBuilder();

        // get nanoseconds
        long ns = time % Util.MILLISECOND_NANOSECONDS;
        time -= ns;
        // get milliseconds
        long ms = time / Util.MILLISECOND_NANOSECONDS;
        time -= ms;
        // get seconds
        long sec = time / Util.SECOND_NANOSECONDS;
        time -= sec;
        // get minutes
        long min = time / Util.MINUTE_NANOSECONDS;

        // format string
        if (min > 0) {
            sb.append(min + " min ");
            sb.append(sec + " sec ");
            sb.append(ms + " ms ");
            sb.append(ns + " ns");
        } else if (sec > 0) {
            sb.append(sec + " sec ");
            sb.append(ms + " ms ");
            sb.append(ns + " ns");
        } else if (ms > 0) {
            sb.append(ms + " ms ");
            sb.append(ns + " ns");
        } else {
            sb.append(ns + " ns");
        }

        return sb.toString();
    }

    /**
     * Formats and prints the necessary output to the console for the BFS and IDS algorithms.
     */
    public static void printOutput(Metrics metrics, Optional<List<Action>> actions) {
        System.out.println();

        // check if timed out
        Util.updateAlgoElapsedTime();
        if (Util.algoElapsedTime > Util.TIMEOUT_NANOSECONDS) {
            System.out.println("Total Nodes Generated: <<??>>");
            System.out.println("Total Nodes Expanded: <<??>>");
            System.out.println("Total Time Taken: >15 min");
            System.out.println("Path Length: Timed Out.");
            System.out.println("Path: Timed Out.");
            return;
        }

        System.out.println("Total Nodes Generated: " + Util.nodeCount);
        System.out.println("Total Nodes Expanded: " + metrics.get("nodesExpanded"));
        System.out.println("Total Time Taken: " + getTime(Util.algoElapsedTime));
        System.out.println("Path Length: " + metrics.get("pathCost"));
        StringBuilder sb = new StringBuilder();
        sb.append("Path: ");
        for (Action action : actions.get()) {
            sb.append(correctActionString(action));
        }
        System.out.println(sb.toString());

        // data tracking
        Util.totalRunTime += Util.algoElapsedTime;
        Util.totalNodeCount += Util.nodeCount;

        Util.resetNodeCount();
    }

    /**
     * Formats and prints the necessary output to the console for the A* algorithms.
     */
    public static void printAStarOutput(SearchAgent<Object, EightPuzzleBoard, Action> agent) {
        System.out.println();

        // check if timed out
        Util.updateAlgoElapsedTime();
        if (Util.algoElapsedTime > Util.TIMEOUT_NANOSECONDS) {
            System.out.println("Total Nodes Generated: <<??>>");
            System.out.println("Total Nodes Expanded: <<??>>");
            System.out.println("Total Time Taken: >15 min");
            System.out.println("Path Length: Timed Out.");
            System.out.println("Path: Timed Out.");
            return;
        }

        System.out.println("Total Nodes Generated: " + Util.nodeCount);
        System.out.println("Total Nodes Expanded: " + agent.getInstrumentation().getProperty("nodesExpanded"));
        System.out.println("Total Time Taken: " + getTime(Util.algoElapsedTime));
        System.out.println("Path Length: " + agent.getInstrumentation().getProperty("pathCost"));
        StringBuilder sb = new StringBuilder();
        sb.append("Path: ");
        for (Action action : agent.getActions()) {
            sb.append(correctActionString(action));
        }
        System.out.println(sb.toString());

        // data tracking
        Util.totalRunTime += Util.algoElapsedTime;
        Util.totalNodeCount += Util.nodeCount;

        Util.resetNodeCount();
    }

    private static String correctActionString(Action action) {
        switch(action.toString()) {
            case "Action[name=Up]":
                return "D";
            case "Action[name=Down]":
                return "U";
            case "Action[name=Left]":
                return "R";
            case "Action[name=Right]":
                return "L";
        }
        return "Error";
    }

    /**
     * Reads a specified input file and creates and returns a list of EightBoardPuzzle
     * objects contained within the file.
     * @param fName
     *          the name of the input file
     */
    public static List<EightPuzzleBoard> getPuzzlesFromFile(String fName) {
        List<EightPuzzleBoard> result = new ArrayList<>();
        try {
            File f = new File(fName);
            FileReader fr = new FileReader(f);
            BufferedReader br = new BufferedReader(fr);
            int c, count = 0;
            int[] state = new int[9];
            while ((c = br.read()) != -1) {
                char ch = (char) c;
                if (validChar(ch) == -1)
                    continue;
                if (count < 8) {
                    // add to state
                    state[count] = validChar(ch);
                    count++;
                } else {
                    // last number - add and then create object and restart count
                    state[count] = validChar(ch);
                    result.add(new EightPuzzleBoard(state));
                    count = 0;
                }
            }
        } catch (FileNotFoundException e) {
            System.out.println("File " + fName + " was not found.");
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println("Unknown error occurred.");
            e.printStackTrace();
        }
        return result;
    }

    /**
     * Makes sure characters read from the input file are valid, and
     * also returns the internal representation for them.
     */
    public static int validChar(char c) {
        switch(c) {
            case '_':
                return 0;
            case '1':
                return 1;
            case '2':
                return 2;
            case '3':
                return 3;
            case '4':
                return 4;
            case '5':
                return 5;
            case '6':
                return 6;
            case '7':
                return 7;
            case '8':
                return 8;
            default:
                return -1;
        }
    }
}