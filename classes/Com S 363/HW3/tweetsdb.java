package coms363;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.sql.*;

import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.border.LineBorder;

/**
 * Author: ComS 363 Teaching Staff
 * Examples of static queries, parameterized queries, and 
 * transactions
 * 
 * Created my own procedure calls and changed the menu text.
 * @author Tyler Evans
 */
public class tweetsdb {

	/**
	 * 	asking for a username and password to access the database.
	 *  @return the array with the username as the first element and password as the second element
	 */
	public static String[] loginDialog() {
	
		
		String result[] = new String[2];
		JPanel panel = new JPanel(new GridBagLayout());
		GridBagConstraints cs = new GridBagConstraints();

		cs.fill = GridBagConstraints.HORIZONTAL;

		JLabel lbUsername = new JLabel("Username: ");
		cs.gridx = 0;
		cs.gridy = 0;
		cs.gridwidth = 1;
		panel.add(lbUsername, cs);

		JTextField tfUsername = new JTextField(20);
		cs.gridx = 1;
		cs.gridy = 0;
		cs.gridwidth = 2;
		panel.add(tfUsername, cs);

		JLabel lbPassword = new JLabel("Password: ");
		cs.gridx = 0;
		cs.gridy = 1;
		cs.gridwidth = 1;
		panel.add(lbPassword, cs);

		JPasswordField pfPassword = new JPasswordField(20);
		cs.gridx = 1;
		cs.gridy = 1;
		cs.gridwidth = 2;
		panel.add(pfPassword, cs);
		panel.setBorder(new LineBorder(Color.GRAY));

		String[] options = new String[] { "OK", "Cancel" };
		int ioption = JOptionPane.showOptionDialog(null, panel, "Login", JOptionPane.OK_OPTION,
				JOptionPane.PLAIN_MESSAGE, null, options, options[0]);
		
		// store the username in the first element of the array.
		// store the password in the second element of the same array.
		
		if (ioption == 0) // pressing the OK button
		{
			result[0] = tfUsername.getText();
			result[1] = new String(pfPassword.getPassword());
		}
		return result;
	}
	
	/**
	 * Calls the findPopularHashtags stored procedure.
	 */
	private static void callFindPopularHashtags(Connection conn, int k, int year_of_interest) {
		if (conn == null) throw new NullPointerException();
		
		try {
			conn.setAutoCommit(false);
			conn.setTransactionIsolation(Connection.TRANSACTION_SERIALIZABLE);
			
			CallableStatement cstmt = conn.prepareCall("{CALL findPopularHashtags(?,?)}");
			
			cstmt.setInt(1, k);
			cstmt.setInt(2, year_of_interest);
			
			ResultSet rs = cstmt.executeQuery();
			ResultSetMetaData rsMetaData = rs.getMetaData();
			String toShow = "";
			
			while (rs.next()) {
				// i+1 indicates the position of the column to obtain the output
				// getString(1) means getting the value of the column 1.
				// concatenate the columns in each row
				for (int i = 0; i < rsMetaData.getColumnCount(); i++) {
					toShow += rs.getString(i + 1) + ", ";
				}
				toShow += "\n";
			}
			// show the dialog box with the returned result by DBMS
			JOptionPane.showMessageDialog(null, toShow);
			
			rs.close();
		} catch (SQLException e) {
			System.out.println("Failed call of findPopularHashtags. Year: " + year_of_interest);
			System.out.println("Check for invalid year input.");
		}
	}
	
	/**
	 * Calls the mostFollowedUsers stored procedure.
	 */
	private static void callMostFollowedUsers(Connection conn, int k, String party) {
		if (conn == null || party == null) throw new NullPointerException();
		
		try {
			conn.setAutoCommit(false);
			conn.setTransactionIsolation(Connection.TRANSACTION_SERIALIZABLE);
			
			CallableStatement cstmt = conn.prepareCall("{CALL mostFollowedUsers(?,?)}");
			
			cstmt.setInt(1, k);
			cstmt.setString(2, party);
			
			ResultSet rs = cstmt.executeQuery();
			ResultSetMetaData rsMetaData = rs.getMetaData();
			String toShow = "";
			
			while (rs.next()) {
				// i+1 indicates the position of the column to obtain the output
				// getString(1) means getting the value of the column 1.
				// concatenate the columns in each row
				for (int i = 0; i < rsMetaData.getColumnCount(); i++) {
					toShow += rs.getString(i + 1) + ", ";
				}
				toShow += "\n";
			}
			// show the dialog box with the returned result by DBMS
			JOptionPane.showMessageDialog(null, toShow);
			
			rs.close();
		} catch (SQLException e) {
			System.out.println("Failed call of callMostFollowedUsers. Party: " + party);
			System.out.println("Check for invalid party input.");
		}
	}
	
	/**
	 * Calls the insertUser stored procedure.
	 */
	private static void callInsertUser(Connection conn, String screen_name, String user_name, String category, String subcategory,
			String state, int numFollowers, int numFollowing) {
		if (conn == null || screen_name == null || user_name == null || category == null || subcategory == null ||
				state == null) throw new NullPointerException();
		
		Integer success = 0;
		try {
			conn.setAutoCommit(false);
			conn.setTransactionIsolation(Connection.TRANSACTION_SERIALIZABLE);
			
			CallableStatement cstmt = conn.prepareCall("{CALL insertUser(?,?,?,?,?,?,?,?)}");
			
			cstmt.setString(1, screen_name);
			cstmt.setString(2, user_name);
			cstmt.setString(3, category);
			cstmt.setString(4, subcategory);
			cstmt.setString(5, state);
			cstmt.setInt(6, numFollowers);
			cstmt.setInt(7, numFollowing);
			cstmt.registerOutParameter(8, Types.INTEGER);
			
			cstmt.executeUpdate();
			
			success = cstmt.getInt(8);
			JOptionPane.showMessageDialog(null, "Success: " + success);
			
			if (success == 1) {
				System.out.println("Successful insertion of " + screen_name);
				conn.commit();
			}
			cstmt.close();
		} catch (SQLException e) {
			System.out.println("Failed call of callInsertUser.");
			System.out.println("Check for invalid input.");
		}
	}
	
	/**
	 * Calls the deleteUser stored procedure.
	 */
	private static void callDeleteUser(Connection conn, String screen_name) {
		if (conn == null || screen_name == null) throw new NullPointerException();
		
		Integer output = 0;
		try {
			conn.setAutoCommit(false);
			conn.setTransactionIsolation(Connection.TRANSACTION_SERIALIZABLE);
			
			CallableStatement cstmt = conn.prepareCall("{CALL deleteUser(?,?)}");
			
			cstmt.setString(1, screen_name);
			cstmt.registerOutParameter(2, Types.INTEGER);
			
			cstmt.executeUpdate();
			
			output = cstmt.getInt(2);
			JOptionPane.showMessageDialog(null, "Output: " + output);
			
			if (output == 0) {
				System.out.println("Successful deletion of " + screen_name);
				conn.commit();
			}
			cstmt.close();
		} catch (SQLException e) {
			System.out.println("Failed call of callDeleteUser.");
			System.out.println("Check for invalid input.");
		}
	}
	
	public static void main(String[] args) {
		/*
			useSSL=false means plain text allowed
			
			String dbServer = "jdbc:mysql://localhost:3306/fooddb?useSSL=false";
			localhost is same as 127.0.0.1
			
			
			Change localhost if you want to use a DBMS on a different computer.
			Change the port number from 3306 if you install your DBMS server on a different port number.
			Change fooddb_ex to the database name you want to use.
			useSSL=true; data are encrypted when sending between DBMS and this program
			
		 * 
		 */
		
		String dbServer = "jdbc:mysql://localhost:3306/practice?useSSL=true";
		String userName = "";
		String password = "";

		// show the login dialog box
		String result[] = loginDialog();
		
		// only use this for debugging
		// String result[] = {"coms363", "XXXX"};
		
		// pass the dialog box to get the result set.
		
		userName = result[0];
		password = result[1];

		Connection conn=null;
		
		// Statement class is for static SQL queries.
		Statement stmt=null;
		
		
		if (result[0]==null || result[1]==null) {
			System.out.println("Terminating: No username nor password is given");
			return;
		}
		try {
			// load JDBC driver
			// must be in the try-catch-block
			Class.forName("com.mysql.cj.jdbc.Driver");
			
			// establish a database connection with the given userName and password
			conn = DriverManager.getConnection(dbServer, userName, password);
			
			// create a static statement
			
			stmt = conn.createStatement();
			String sqlQuery = "";

			// Menu options
			String option = "";
			String instruction = "Enter a: Find k hashtags that appeared in the most # of states in a given year." + "\n"
					+ "Enter b: Find k most followed users of a given party." + "\n"
					+ "Enter c: Insert a new Twitter user into the users relation." + "\n"
					+ "Enter d: Delete a user from the users relation given the screen name of the user." + "\n"
					+ "Enter e or other key: Quit Program";

			while (true) {
				// show the above menu options
				option = JOptionPane.showInputDialog(instruction);
				// Reset the autocommit to commit per SQL statement
				// This is for the other SQL queries to be one independently treated as one unit.
				// set the default isolation level to the default.
				
				conn.setAutoCommit(true);
				conn.setTransactionIsolation(Connection.TRANSACTION_REPEATABLE_READ);
				
				if (option.equals("a")) {
					int k = Integer.parseInt(JOptionPane.showInputDialog("Enter k:"));
					int year_of_interest = Integer.parseInt(JOptionPane.showInputDialog("Enter year:"));
					callFindPopularHashtags(conn, k, year_of_interest);
				} else if (option.equals("b")) {
					int k = Integer.parseInt(JOptionPane.showInputDialog("Enter k:"));
					String party = JOptionPane.showInputDialog("Enter party:");
					callMostFollowedUsers(conn, k, party);
				} else if (option.equals("c")) {
					String screen_name = JOptionPane.showInputDialog("Enter screen name:");
					String user_name = JOptionPane.showInputDialog("Enter user name:");
					String category = JOptionPane.showInputDialog("Enter category:");
					String subcategory = JOptionPane.showInputDialog("Enter subcategory:");
					String state = JOptionPane.showInputDialog("Enter state:");
					int numFollowers = Integer.parseInt(JOptionPane.showInputDialog("Enter num followers:"));
					int numFollowing = Integer.parseInt(JOptionPane.showInputDialog("Enter num following:"));
					callInsertUser(conn, screen_name, user_name, category, subcategory, state, numFollowers, numFollowing);
				} else if (option.equals("d")) {
					String screen_name = JOptionPane.showInputDialog("Enter screen name:");
					callDeleteUser(conn, screen_name);
				} else {
					break;
				}
			}
			// close the statement
			if (stmt != null) stmt.close();
			// close the connection
			if (conn != null) conn.close();
		} catch (Exception e) {
			
			System.out.println("Program terminates due to errors or user cancellation");
			// e.printStackTrace(); // enable this to see the debugging errors
		}
	}
}
