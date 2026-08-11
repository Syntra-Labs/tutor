package protocols;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

//Environment state management
class EnvironmentState {
 private List<PotentialField> potentialFields;
 private Map<String, Object> stateVariables;
 
 public EnvironmentState() {
     this.potentialFields = new ArrayList<>();
     this.stateVariables = new HashMap<>();
 }
 
 public void addPotentialField(PotentialField field) {
     potentialFields.add(field);
 }
 
 public void setStateVariable(String key, Object value) {
     stateVariables.put(key, value);
 }
}