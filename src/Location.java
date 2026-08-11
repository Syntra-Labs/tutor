import java.util.ArrayList;

public class Location {
	private String description;
	private ArrayList<String> actions;
	
	public Location(String description) {
		this.description = description;
		this.actions = new ArrayList<>();
	}
	
	public String getDescription() {
		return description;
	}
	
	public void addAction(String action) {
		actions.add(action);
	}
	
	public ArrayList<String> getActions() {
		return actions;
	}

}
