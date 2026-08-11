/**
 * Date: February 15, 2025
 * Authors: Yassine Amraoui (041044570) & Sheilla Shojaie (41171088)
 * Section Number: 303
 * Lab Professor: Paulo Sousa
 * Description: CST8132 25W: Object-Oriented Programming | Assignment 01: BookList
 * 
 * A BookManager class to serve as a controller for managing a list of best-selling books.
 * It provides a console menu to select management options which include loading book fields from a CSV file, 
 * adding/editing/deleting/searching for Books, saving the list to a CSV file, and exiting the application.
 * It uses the BookList class to operate on the list.
 * 
 */
package bestseller;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;

public class BookManager {

	private BookList bookList = new BookList();
	private static final Scanner scanner = new Scanner(System.in);

	// Menu option constants
	private static final int LOAD_BOOKLIST = 1;
	private static final int PRINT_BOOKLIST = 2;
	private static final int ADD_BOOK = 3;
	private static final int EDIT_BOOK = 4;
	private static final int DELETE_BOOK = 5;
	private static final int SAVE_BOOKLIST = 6;
	private static final int SEARCH_BOOKLIST = 7;
	private static final int EXIT = 8;
	private static final String DEFAULT_FILE = "amazonbooks.csv";

	public static void main(String[] args) {

		BookManager manager = new BookManager();
		try {
			manager.manageBookList();
		} catch (Exception e) {
			System.err.println("Error running application: " + e.getMessage());
		}
	}

	/*
	 * Provides a console menu to select management options to perform book list
	 * operations.
	 */
	public void manageBookList() throws BookException {
		int option = 0;
		do {
			try {
				showMenu();

				// Validate user input is an integer
				if (!scanner.hasNextInt()) {
					System.err.println("Invalid entry: enter an integer between 1 and 8.");
				    scanner.nextLine();
				    continue;
				}

				option = scanner.nextInt();
				scanner.nextLine();

				// Validate user input is within range
				if (option < LOAD_BOOKLIST || option > EXIT) {
					System.err.println("Invalid entry: enter an integer between 1 and 8.");
				}

				// Execute operation according to user selection
				switch (option) {
				case LOAD_BOOKLIST:
					loadBookList();
					break;
				case PRINT_BOOKLIST:
					printBookList();
					break;
				case ADD_BOOK:
					addBook();
					break;
				case EDIT_BOOK:
					editBook();
					break;
				case DELETE_BOOK:
					deleteBook();
					break;
				case SAVE_BOOKLIST:
					saveBookList();
					break;
				case SEARCH_BOOKLIST:
					searchBook();
					break;
				case EXIT:
					exit();
					return;
				}
			} catch (BookException e) {
				System.err.println("Error managing book list: " + e.getMessage());
			}
		} while (true);
	}

	/*
	 * Prompts for the book list CSV file name and loads the book list.
	 */
	public void loadBookList() throws BookException {
		System.out.println("Enter the name of the booklist file or press Enter to use the default file: ");
		String csvFile = scanner.nextLine();
		if (csvFile.isEmpty()) {
			csvFile = DEFAULT_FILE;
		}
		try {
			bookList.loadBookList(csvFile);
		} catch (BookException e) {
			throw new BookException("Error loading book list: " + e.getMessage());
		}
		System.out.println("Book list created successfully!");
	}

	/*
	 * Prints the list of books.
	 */
	public void printBookList() throws BookException {
		try {
		bookList.printBookList();
		} catch (BookException e) {
			throw new BookException("Error printing book list: " + e.getMessage());
		}
	}

	/*
	 * Prompts to enter the fields of a new book and adds it to the book list.
	 */
	public void addBook() throws BookException {

		System.out.println("Enter the book data:");
		String[] fields = new String[6];
		
		try {
			// Take user input for each field
			System.out.print("- Name: ");
			fields [0] = scanner.nextLine();
			System.out.print("- Author: ");
			fields [1] = scanner.nextLine();
			System.out.print("- Original Language: ");
			fields [2] = scanner.nextLine();
			System.out.print("- First Published: ");
			fields [3] = scanner.nextLine();
			System.out.print("- Millions of Sales: ");
			fields [4] = scanner.nextLine();
			System.out.print("- Genre: ");
			fields [5] = scanner.nextLine();
			
			// Validate user input against empty strings
			for (int i = 0; i < 5; i++) {
				if (fields[i] == null || fields[i].isEmpty()) {
					throw new BookException("All fields except genre must be filled.");
				}
			}
			
			// Validate user input is a valid year
			if (!fields [3].matches("-?\\d+?")) {
				throw new BookException("Invalid value for 'First Published'. Enter an integer.");
			}
			else if (Integer.parseInt(fields [3]) <= 0 || Integer.parseInt(fields [3]) > 2025)
				throw new BookException("Enter a valid year between 0 and 2025.");
			
			// Validate user input is a valid sales number
			if (!fields [4].matches("-?\\d+(\\.\\d+)?")) {
				throw new BookException("Invalid value for 'Millions of Sales'. Enter a numeric value.");
			}
			else if (Float.parseFloat(fields [4]) < 0)
				throw new BookException("Enter valid sales number.");
			
			// Create book using fields
			Book newBook = bookList.createBook(new ArrayList<>(Arrays.asList(fields)));
			bookList.addBook(newBook);
			System.out.println("Book added successfully!");
		} catch (BookException e) {
			throw new BookException("Error adding book: " + e.getMessage());
		}
	}

	/*
	 * Prompts to enter the index of the book to delete and deletes it from the book
	 * list.
	 */
	public void deleteBook() throws BookException {
		System.out.println("Enter the index of the book to delete:");

		// Validate user input is an integer
		if (!scanner.hasNextInt()) {
			System.err.println("Enter valid integer index.");
			scanner.nextLine();
		}

		int index = scanner.nextInt();
		scanner.nextLine();

		// Validate user input is within range
		if (index < 0 || index >= bookList.getSize()) {
			System.err.println("Enter valid index.");
		}

		try {
			bookList.deleteBook(index);
			System.out.println("Book [" + index + "] deleted successfully!");
		} catch (BookException e) {
			throw new BookException("Error deleting book: " + e.getMessage());
		}
	}

	/*
	 * Prompts to enter the index of the book to edit and edits its fields.
	 */
	public void editBook() throws BookException {
		System.out.println("Enter the index of the book to edit:");

		try {
			
			// Validate index is an integer and is within range
			if (!scanner.hasNextInt()) {
				throw new BookException("Enter valid index.");
			}
			
			int index = scanner.nextInt();
			scanner.nextLine();
			
			if (index < 0 || index >= bookList.getSize()) {
				throw new BookException("Enter valid index.");
			}
			
			System.out.println("Enter the book data: ");
			String[] fields = new String[6];
			
			// Take user input for each field to edit book
			System.out.print("- Name: ");
			fields [0] = scanner.nextLine();
			System.out.print("- Author: ");
			fields [1] = scanner.nextLine();
			System.out.print("- Original Language: ");
			fields [2] = scanner.nextLine();
			System.out.print("- First Published: ");
			fields [3] = scanner.nextLine();
			System.out.print("- Millions of Sales: ");
			fields [4] = scanner.nextLine();
			System.out.print("- Genre: ");
			fields [5] = scanner.nextLine();
			
			// Validate user input against empty strings
			for (int i = 0; i < 5; i++) {
				if (fields[i] == null || fields[i].isEmpty()) {
					throw new BookException("All fields except genre must be filled.");
				}
			}
			
			// Validate user input is a valid year
			if (!fields [3].matches("-?\\d+?")) {
				throw new BookException("Invalid value for 'First Published'. Enter an integer.");
			}
			else if (Integer.parseInt(fields [3]) <= 0 || Integer.parseInt(fields [3]) > 2025)
				throw new BookException("Enter a valid year between 0 and 2025.");
			
			// Validate user input is a valid sales number
			if (!fields [4].matches("-?\\d+(\\.\\d+)?")) {
				throw new BookException("Invalid value for 'Millions of Sales'. Enter a numeric value.");
			}
			else if (Float.parseFloat(fields [4]) < 0)
				throw new BookException("Enter valid sales number.");
			
			bookList.editBook(index, new ArrayList<>(Arrays.asList(fields)));
			System.out.println("Book edited successfully!");
		} catch (BookException e) {
			throw new BookException("Error editing book: " + e.getMessage());
		}
	}

	/*
	 * Displays the console menu of book list management options.
	 */
	public void showMenu() {
		System.out.println("===============================================");
		System.out.println("|| Menu - Best Sellers OOP/A1                ||");
		System.out.println("|| Team: Yassine Amraoui, Sheilla Shojaie    ||");
		System.out.println("===============================================");
		System.out.println("1. Load book list");
		System.out.println("2. Show book list");
		System.out.println("3. Add book");
		System.out.println("4. Edit a book");
		System.out.println("5. Delete a book");
		System.out.println("6. Save book list");
		System.out.println("7. Search in the list");
		System.out.println("8. Exit");
		System.out.print("Choose an option: ");
	}

	/*
	 * Prompts for the name of the CSV file to save the book list into and saves it
	 * to that file.
	 */
	public void saveBookList() throws BookException {
		System.out.println("Name of file to save book list:");
		try {
		String csvFile = scanner.nextLine();
		bookList.saveBookList(csvFile);
		} catch (BookException e) {
			throw new BookException("Error saving book: " + e.getMessage());
		}
	}

	/*
	 * Prompts to enter a search term and searches the book list for matches.
	 */
	public void searchBook() throws BookException {
		System.out.println("Enter one string for case-insensitive search in the list:");
		try {
		String input = scanner.nextLine();
		// Perform case-insensitive search
		System.out.println("Results from search...");
		bookList.searchBook(input.toLowerCase());
		} catch (BookException e) {
			throw new BookException("Error searching book list: " + e.getMessage());
		}
	}

	/*
	 * Exits the application.
	 */
	public void exit() {
		System.out.println(" ================================");
		System.out.println("||     [Application ended]     ||");
		System.out.println(" ================================");
	}

}
