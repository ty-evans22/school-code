import search.csp.Assignment;
import search.csp.CSP;
import search.csp.Domain;
import search.csp.Variable;
import search.csp.examples.NotEqualConstraint;
import search.csp.solver.FlexibleBacktrackingSolver;
import search.csp.solver.MinConflictsSolver;
import search.csp.solver.inference.AC3Strategy;
import search.csp.solver.inference.ForwardCheckingStrategy;

import java.util.*;

/**
 * @author Tyler Evans
 */
public class Main {
    public static Random rand = new Random();
    public static int GRID_SIZE = 500;
    public static int CIRCLE_SIZE = 20;
    public static List<Point> col;
    public static long bt3StartTime, bt4StartTime, fc3StartTime, fc4StartTime, mac3StartTime, mac4StartTime, min3StartTime, min4StartTime;
    public static long bt3Time, bt4Time, fc3Time, fc4Time, mac3Time, mac4Time, min3Time, min4Time;

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Incorrect number of arguments.");
            return;
        }

        // construct maps
        List<Variable> vars = variables(Integer.parseInt(args[0]));
        MapCSP3 map3 = new MapCSP3(vars);
        MapCSP4 map4 = new MapCSP4(vars);

        // backtracking w/ no inference
        bt3StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> results3 = new FlexibleBacktrackingSolver<Variable, String>().solve(map3);
        bt3Time = System.nanoTime() - bt3StartTime;

        bt4StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> results4 = new FlexibleBacktrackingSolver<Variable, String>().solve(map4);
        bt4Time = System.nanoTime() - bt4StartTime;

        // backtracking w/ forward checking
        fc3StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> fcresults3 = new FlexibleBacktrackingSolver<Variable, String>().set(new ForwardCheckingStrategy<>()).solve(map3);
        fc3Time = System.nanoTime() - fc3StartTime;

        fc4StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> fcresults4 = new FlexibleBacktrackingSolver<Variable, String>().set(new ForwardCheckingStrategy<>()).solve(map4);
        fc4Time = System.nanoTime() - fc4StartTime;

        // backtracking w/ MAC
        mac3StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> acresults3 = new FlexibleBacktrackingSolver<Variable, String>().set(new AC3Strategy<>()).solve(map3);
        mac3Time = System.nanoTime() - mac3StartTime;

        mac4StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> acresults4 = new FlexibleBacktrackingSolver<Variable, String>().set(new AC3Strategy<>()).solve(map4);
        mac4Time = System.nanoTime() - mac4StartTime;

        // min-conflict
        min3StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> minresults3 = new MinConflictsSolver<Variable, String>(100).solve(map3);
        min3Time = System.nanoTime() - min3StartTime;

        min4StartTime = System.nanoTime();
        Optional<Assignment<Variable, String>> minresults4 = new MinConflictsSolver<Variable, String>(100).solve(map4);
        min4Time = System.nanoTime() - min4StartTime;

        // display times
        System.out.println("BT K = 3 Time: " + (bt3Time / 1000000.0) + " ms");
        System.out.println("BT K = 4 Time: " + (bt4Time / 1000000.0) + " ms");
        System.out.println("FC K = 3 Time: " + (fc3Time / 1000000.0) + " ms");
        System.out.println("FC K = 4 Time: " + (fc4Time / 1000000.0) + " ms");
        System.out.println("MAC K = 3 Time: " + (mac3Time / 1000000.0) + " ms");
        System.out.println("MAC K = 4 Time: " + (mac4Time / 1000000.0) + " ms");
        System.out.println("MIN K = 3 Time: " + (min3Time / 1000000.0) + " ms");
        System.out.println("MIN K = 4 Time: " + (min4Time / 1000000.0) + " ms");

        // debugging stuff - uncomment to show actual results
//        if (results3.isPresent()) {
//            System.out.println("K = 3 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + results3.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 3.");
//        }
//
//        if (results4.isPresent()) {
//            System.out.println("K = 4 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + results4.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 4.");
//        }

//        if (fcresults3.isPresent()) {
//            System.out.println("K = 3 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + fcresults3.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 3");
//        }
//
//        if (fcresults4.isPresent()) {
//            System.out.println("K = 4 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + fcresults4.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 4");
//        }

//        if (acresults3.isPresent()) {
//            System.out.println("K = 3 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + acresults3.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 3");
//        }
//
//        if (acresults4.isPresent()) {
//            System.out.println("K = 4 results:");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + acresults4.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 4");
//        }

//        if (minresults3.isPresent()) {
//            System.out.println("K = 3 results");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + minresults3.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 3");
//        }
//
//        if (minresults4.isPresent()) {
//            System.out.println("K = 4 results");
//            for (int i = 0; i < vars.size(); i++) {
//                System.out.println(vars.get(i).getName() + ": " + minresults4.get().getValue(vars.get(i)));
//            }
//        } else {
//            System.out.println("Unable to solve for k = 4");
//        }
    }

    public static List<Variable> variables(int n) {
        List<Variable> results = new ArrayList<Variable>();

        // generate the points
        Main.col = Main.getPoints(n);
        Main.buildGraph();

        // debug testing for point and neighbor locations
//        for (int i = 0; i < n; i++) {
//            System.out.println("X: " + Main.col.get(i).getX() + " Y: " + Main.col.get(i).getY());
//            System.out.println("Num neighbors: " + Main.col.get(i).neighbors.size());
//            for (int j = 0; j < Main.col.get(i).neighbors.size(); j++) {
//                System.out.println("     X: " + Main.col.get(i).neighbors.get(j).getX() + " Y: " + Main.col.get(i).neighbors.get(j).getY());
//            }
//        }

        for (int i = 0; i < n; i++) {
            results.add(new Variable("P" + i));
        }
        return results;
    }

    public static void buildGraph() {
        Point[] helper = new Point[col.size()]; //Helper maintain the liveness of the nodes
        int lastIdx = helper.length - 1;
        for(int i = 0; i < col.size(); i++) {
            col.get(i).init();
            helper[i] = col.get(i);
        }
        while(lastIdx > 0) {
            int i = rand.nextInt(lastIdx + 1);
            if(!helper[i].getConnection()) {
                swap(i, lastIdx, helper);
                lastIdx--;
            }
        }
    }

    public static List<Point> getPoints(int size) {
        if(size < 0) {
            return null;
        }
        List<Point> result = new ArrayList<Point>();
        HashSet<Integer> duplicatedFounder = new HashSet<Integer>();
        for(int i = 0; i < size; i++) {
            int x = rand.nextInt(GRID_SIZE - CIRCLE_SIZE / 2);
            int y = rand.nextInt(GRID_SIZE - CIRCLE_SIZE / 2);
            //Avoid dulplicate
            while(duplicatedFounder.contains(cantorPairing(x, y))) {
                x = rand.nextInt(GRID_SIZE - CIRCLE_SIZE / 2);
                y = rand.nextInt(GRID_SIZE - CIRCLE_SIZE / 2);
            }
            result.add(new Point(x, y));
        }
        return result;
    }

    public static int cantorPairing(int x, int y) {
        return ((x + y) * (x + y + 1) + y) / 2;
    }

    private static void swap(int i, int j, Point[] array) {
        Point t = array[i];
        array[i] = array[j];
        array[j] = t;
    }
}

class MapCSP3 extends CSP<Variable, String> {
    public static final String RED = "RED";
    public static final String GREEN = "GREEN";
    public static final String BLUE = "BLUE";

    public MapCSP3(List<Variable> variables) {
        super(variables);

        Domain<String> colors = new Domain<>(RED, GREEN, BLUE);
        for (Variable var : getVariables())
            setDomain(var, colors);

        // add constraints between neighbors
//        System.out.println("K = 3");
        for (int i = 0; i < variables.size(); i++) {
            for (int j = i + 1; j < variables.size(); j++) {
                if (!Point.isIntersected(Main.col.get(i), Main.col.get(j))) {
//                    System.out.println("Neighbors: " + i + " & " + j);
                    addConstraint(new NotEqualConstraint<>(variables.get(i), variables.get(j)));
                }
            }
        }
    }
}

class MapCSP4 extends CSP<Variable, String> {
    public static final String RED = "RED";
    public static final String GREEN = "GREEN";
    public static final String BLUE = "BLUE";
    public static final String YELLOW = "YELLOW";

    public MapCSP4(List<Variable> variables) {
        super(variables);

        Domain<String> colors = new Domain<>(RED, GREEN, BLUE, YELLOW);
        for (Variable var : getVariables())
            setDomain(var, colors);

        // add constraints between neighbors
//        System.out.println("K = 4");
        for (int i = 0; i < variables.size(); i++) {
            for (int j = i + 1; j < variables.size(); j++) {
                if (!Point.isIntersected(Main.col.get(i), Main.col.get(j))) {
//                    System.out.println("Neighbors: " + i + " & " + j);
                    addConstraint(new NotEqualConstraint<>(variables.get(i), variables.get(j)));
                }
            }
        }
    }
}

class Point {
    private int x;
    private int y;
    private int remainValues;
    List<Point> neighbors;
    private PriorityQueue<Point> neighborCandidates;

    public Point(int x, int y) {
        this.x = x;
        this.y = y;
        this.remainValues = 4;
        final int tx = x;
        final int ty = y;
        neighbors = new ArrayList<Point>();
        neighborCandidates = new PriorityQueue<Point>(Main.GRID_SIZE, new Comparator<Point>() {
            @Override
            public int compare(Point a, Point b) {
                int aVal = (a.getX() - tx) * (a.getX() - tx) + (a.getY() - ty) * (a.getY() - ty);
                int bVal = (b.getX() - tx) * (b.getX() - tx) + (b.getY() - ty) * (b.getY() - ty);
                if(aVal == bVal) {
                    return 0;
                }
                return aVal < bVal? -1 : 1;
            }
        });
    }

    public void init() {
        for (int i = 0; i < Main.col.size(); i++) {
            if (Main.col.get(i) != this) {
                neighborCandidates.offer(Main.col.get(i));
            }
        }
    }

    public boolean getConnection() {
        while(!neighborCandidates.isEmpty()) {
            Point candidate = neighborCandidates.poll();
            if(!isIntersected(this, candidate)) {
                this.addNeighbor(candidate);
                candidate.addNeighbor(this);
                return true;
            }
            candidate.removeCandidate(this); //If there is an intersection, this is no more to be a candidate of candidate
        }
        return false;
    }

    //Intersection algo, please refer to http://paulbourke.net/geometry/pointlineplane/
    public static boolean isIntersected(Point one, Point two) {
        for(Point p : Main.col) {
            for(Point pNei : p.neighbors) {
                if(one == p || one == pNei || two == p || two == pNei) {
                    continue;
                }
                int denominator = (pNei.getY() - p.getY()) * (two.getX() - one.getX()) - (pNei.getX() - p.getX()) * (two.getY() - one.getY());
                if(denominator == 0) {
                    continue;
                }
                double uA = (pNei.getX() - p.getX()) * (one.getY() - p.getY()) - (pNei.getY() - p.getY()) * (one.getX() - p.getX());
                double uB = (two.getX() - one.getX()) * (one.getY() - p.getY()) - (two.getY() - one.getY()) * (one.getX() - p.getX());
                uA /= denominator;
                uB /= denominator;
                if(0.0 <= uA && uA <= 1.0 && 0.0 <= uB && uB <= 1.0) {
                    return true;
                }
            }
        }
        return false;
    }

    public void setX(int x) {
        this.x = x;
    }

    public void setY(int y) {
        this.y = y;
    }

    public void setRemainValues(int val) {
        this.remainValues = val;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public int getRemainValues() {
        return remainValues;
    }

    public void addNeighbor(Point nei) {
        this.neighborCandidates.remove(nei);
        this.neighbors.add(nei);
    }

    public void removeCandidate(Point can) {
        neighborCandidates.remove(can);
    }
}
