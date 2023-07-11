package util;

/**
 * Utility class with useful functions and declarations for implementing the search algorithms.
 *
 * @author Tyler Evans
 */
public class Util {
    // constants for converting from nanoseconds
    public static final long MILLISECOND_NANOSECONDS = 1000000L;
    public static final long SECOND_NANOSECONDS = 1000000000L;
    public static final long MINUTE_NANOSECONDS = 60000000000L;
    public static final long TIMEOUT_NANOSECONDS = 900000000000L;

    // storage for algorithm start and end times
    public static long algoStartTime = 0;
    public static long algoEndTime = 0;
    public static long algoElapsedTime = 0;

    public static long nodeCount = 0;

    public static long totalNodeCount = 0;
    public static long totalRunTime = 0;

    /**
     * Returns the time, in nanoseconds, elapsed since the algorithm execution began.
     *
     * @return the time, in nanoseconds, elapsed since the algorithm execution began
     */
    public static long currElapsedTime() {
        return System.nanoTime() - algoStartTime;
    }

    /**
     * Stores the value for the current system time, in nanoseconds, in algoStartTime. This signifies the time that
     * the algorithm's execution first began.
     */
    public static void updateAlgoStartTime() {
        algoStartTime = System.nanoTime();
    }

    /**
     * Stores the value for the current system time, in nanoseconds, in algoEndTime. This signifies the time that the
     * algorithm's execution finished.
     */
    public static void updateAlgoEndTime() {
        algoEndTime = System.nanoTime();
    }

    /**
     * Calculates the time, in nanoseconds, that the algorithm took to execute and stores the value in algoElapsedTime.
     */
    public static void updateAlgoElapsedTime() {
        algoElapsedTime = algoEndTime - algoStartTime;
    }

    /**
     * Resets the algorithm time variables to their initial state (0).
     */
    public static void resetAlgoTimes() {
        algoStartTime = 0;
        algoEndTime = 0;
        algoElapsedTime = 0;
    }

    public static void resetNodeCount() {
        nodeCount = 0;
    }

    public static long getAvgRunTime(int numPuzzles) {
        return totalRunTime / numPuzzles;
    }

    public static long getAvgNodeCount(int numPuzzles) {
        return totalNodeCount / numPuzzles;
    }
}
