/**
 * Date: February 15, 2025
 * Authors: Yassine Amraoui (041044570) & Sheilla Shojaie (41171088)
 * Section Number: 303
 * Lab Professor: Paulo Sousa
 * Description: CST8132 25W: Object-Oriented Programming | Assignment 01: BookList
 * 
 * A BookList class to manage a list of best-selling books.
 * It provides methods to load book fields from a CSV file, add/edit/delete/search for Books,
 * save the list to a CSV file, and print the list.
 * The list uses the Book class to represent individual books.
 * 
 */
package bestseller;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class BookList {
	// Header line read from loaded file
	private ArrayList<String> title;

	private ArrayList<Book> bestsellers;

	/*
	 * Constructs an empty BookList.
	 */
	public BookList() {
		bestsellers = new ArrayList<>();
	}

	/*
	 * Loads the book list from a CSV file.
	 */
	public void loadBookList(String csvFile) throws BookException {

		bestsellers.clear();

		try (BufferedReader reader = new BufferedReader(new FileReader(csvFile))) {
			String line;

			// Skip the header line
			String header = reader.readLine();
			title =  lineReader(header);

			// Process each line in the CSV file, extracting fields and creating Books
			while ((line = reader.readLine()) != null) {

				ArrayList<String> fields = lineReader(line);

				Book book = createBook(fields);
				bestsellers.add(book);
			}
		} catch (FileNotFoundException e) {
			throw new BookException("File not found: " + csvFile);
		} catch (IOException e) {
			throw new BookException("Error reading file: " + e.getMessage());
		} catch (Exception e) {
			throw new BookException("Error loading book list: " + e.getMessage());
		}
	}

	/*
	 * Reads a line from the CSV file and extracts fields.
	 */
	public ArrayList<String> lineReader(String line) {
		ArrayList<String> fields = new ArrayList<String>();
		String field = "";
		boolean inQuotes = false;
		char ch;
		for (int i = 0; i < line.length(); i++) {
			ch = line.charAt(i);

			// If the character is a quote, toggle the inQuotes flag
			if (ch == '"') {
				inQuotes = !inQuotes;
			// If it's a comma and not inside quotes, it's a field delimiter
			} else if (ch == ',' && !inQuotes) {
				fields.add(field.trim());
				field = "";
			// Else, build the current field
			} else {
				field = field + ch;
			}
		}
		// Add the final field
		fields.add(field.strip());

		return fields;
	}

	/*
	 * Adds a new Book to the list.
	 */
	public void addBook(Book b) throws BookException {
		try { 
			bestsellers.add(b);
		} catch (Exception e) {
			throw new BookException("Error adding book: " + e.getMessage());
		}
	}

	/*
	 * Creates a new Book from the parsed fields.
	 */
	public Book createBook(ArrayList<String> fields) throws BookException {
		try {
			// Validate that there are 6 fields
			if (fields.size() != 6) {
				throw new BookException("Expected 6 fields and received " + fields.size());
			}

			String name = fields.get(0);
			String author = fields.get(1);
			String originalLanguage = fields.get(2);

			// Validate year
			int firstPublished = Integer.parseInt(fields.get(3));
			if (firstPublished <= 0 || firstPublished > 2025) {
				throw new BookException("Enter valid year between 0 and 2025.");
			}

			// Validate sales in millions number
			float millionSales = Float.parseFloat(fields.get(4));
			if (millionSales < 0) {
				throw new BookException("Enter valid sales number.");
			}

			String genre = fields.get(5);

			return new Book(bestsellers.size(), name, author, originalLanguage, firstPublished, millionSales, genre);
		} catch (NumberFormatException e) {
			throw new BookException("Enter valid number: " + e.getMessage());
		} catch (Exception e) {
			throw new BookException("Error creating book: " + e.getMessage());
		}
	}

	/*
	 * Deletes a Book from the list.
	 */
	public void deleteBook(int index) throws BookException {
		if (index < 0 || index >= bestsellers.size()) {
			throw new BookException("Enter valid index between 0 and " + (bestsellers.size() - 1) + ".");
		}

		bestsellers.remove(index);

		// Update indices of Books listed after this Book
		for (int i = index; i < bestsellers.size(); i++) {
			findBookByIndex(i).setIndex(i);
		}
	}

	/*
	 * Edits the fields of a Book at the specified Index.
	 */
	public void editBook(int index, ArrayList<String> fields) throws BookException {
		// Validate index is within range
		if (index < 0 || index >= bestsellers.size()) {
			throw new BookException("Enter a valid index between 0 and " + (bestsellers.size() - 1) + ".");
		}

		Book book = findBookByIndex(index);

		String name = fields.get(0);
		String author = fields.get(1);
		String originalLanguage = fields.get(2);
		int firstPublished;
		float millionSales;
		String genre = fields.get(5);

		try {
			firstPublished = Integer.parseInt(fields.get(3));
			millionSales = Float.parseFloat(fields.get(4));
		} catch (NumberFormatException e) {
			throw new BookException("Invalid number: " + e.getMessage());
		}

		book.setIndex(index);
		book.setName(name);
		book.setAuthor(author);
		book.setOriginalLanguage(originalLanguage);
		book.setFirstPublished(firstPublished);
		book.setMillionSales(millionSales);
		book.setGenre(genre);
	}

	/*
	 * Returns a Book at the specified Index.
	 */
	public Book findBookByIndex(int index) {
		return bestsellers.get(index);
	}

	/*
	 * Prints the list of Books.
	 */
	public void printBookList() throws BookException {
		// Validate book list contains Books
		if (bestsellers == null || bestsellers.isEmpty()) {
			throw new BookException("Book list is empty. Please load a book list and try again.");
		}
		
		System.out.println("Index, " + String.join(", ", title));
		int index = 0;
		for (index = 0; index < bestsellers.size(); index++) {
			System.out.println(bestsellers.get(index));
		}
	}

	/*
	 * Saves the book list to a CSV file.
	 */
	public void saveBookList(String csvFile) throws BookException {
		try (FileWriter fileWriter = new FileWriter(csvFile)) {

			// Write header line to file
			fileWriter.append(String.join(",", title) + "\n");

			for (Book book : bestsellers) {
				String name = book.getName();
				String author = book.getAuthor();
				String originalLanguage = book.getOriginalLanguage();
				String firstPublished = String.valueOf(book.getFirstPublished());
				String formattedMillionSales = book.formatMillionSales(book.getMillionSales());
				String genre = book.getGenre();

				// Handle special characters in fields
				name = handleField(name);
				author = handleField(author);
				genre = handleField(genre);

				// Construct and write line to file
				String line = name + "," + author + "," + originalLanguage + "," + firstPublished + "," + formattedMillionSales
						+ "," + genre + "\n";
				fileWriter.append(line);
			}

			System.out.println("File " + csvFile + " created.");
		} catch (IOException e) {
			throw new BookException("Error saving book list: " + e.getMessage());
		}
	}

	/*
	 * Handles special characters in CSV fields.
	 */
	private static String handleField(String field) {
		if (field.contains("\"") || field.contains(",")) {
			field = "\"" + field.replace("\"", "\"\"") + "\"";
		}
		return field;
	}

	/*
	 * Searches for Books matching the specified search term.
	 */
	public void searchBook(String searchTerm) throws BookException {
		ArrayList<Book> matches = new ArrayList<>();
		// Validate user input contains a search term
		if (searchTerm == null || searchTerm.isEmpty()) {
			throw new BookException("Search term cannot be empty.");
		}

		// Find Books matching the search term
		for (Book book : bestsellers) {
			if (String.valueOf(book.getIndex()).contains(searchTerm)
					|| book.getName().toLowerCase().contains(searchTerm)
					|| book.getAuthor().toLowerCase().contains(searchTerm)
					|| book.getOriginalLanguage().toLowerCase().contains(searchTerm)
					|| String.valueOf(book.getFirstPublished()).contains(searchTerm)
					|| String.valueOf(book.getMillionSales()).contains(searchTerm)
					|| book.getGenre().toLowerCase().contains(searchTerm)) {
				matches.add(book);
			}
		}

		if (matches.isEmpty()) {
			System.out.println("No books found.");
		} else {
			for (int i = 0; i < matches.size(); i++) {
				System.out.println(matches.get(i));
			}
		}
	}

	/*
	 * Returns the size of the book list.
	 */
	public int getSize() {
		return bestsellers.size();
	}
}