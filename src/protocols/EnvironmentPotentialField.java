package protocols;

import java.util.List;

//Environmental potential field calculator
class EnvironmentPotentialField {
 public EnvironmentState calculatePotentialField(List<Actor> actors) {
     EnvironmentState state = new EnvironmentState();
     
     for (Actor actor : actors) {
         Point pos = actor.getPosition();
         // Simple potential field calculation based on distance
         PotentialField field = new PotentialField(pos, 1.0 / (pos.x * pos.x + pos.y * pos.y));
         state.addPotentialField(field);
     }
     
     return state;
 }
}