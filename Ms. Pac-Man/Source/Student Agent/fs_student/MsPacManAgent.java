package fs_student;

import java.util.ArrayList;
import game.controllers.PacManController;
import game.core.G;
import game.core.Game;

public class MsPacManAgent implements PacManController
{
	// instance variables
	int pacman = 0;
	int[] ghost_Distances = new int[Game.NUM_GHOSTS];
	int[] ghost_Pos = new int[Game.NUM_GHOSTS];

	// evaluate ghosts to determine next action -- defaulting to follow pills if no action is prompted by ghosts
	public int getAction(Game game, long time)
	{
		// update pacman location, ghost position, and ghost distance -- this data will also be used in doBehavior
		pacman = game.getCurPacManLoc();
		for (int i = 0; i < Game.NUM_GHOSTS; i++)
		{
			ghost_Pos[i] = game.getCurGhostLoc(i);
			ghost_Distances[i] = (int) game.getEuclideanDistance(ghost_Pos[i], pacman);
		}

		// evaluate ghosts
		int eval = 0;
		for (int i = 0; i < Game.NUM_GHOSTS; i++)
		{																
			// determine if we should do eat ghost
			if (game.isEdible(i) && ghost_Distances[i] < 100)
			{
				eval = -1;
			}
			
			// determine if we should do avoid ghost
			if (ghost_Pos[i] != game.getInitialGhostsPosition())
			{
				if (!game.isEdible(i) && ghost_Distances[i] < 5) // enemy ghost is close - should do avoid ghost and break out of evaluation
				{
					eval = 1;
					break;	
				}
			}
		}

		// Check results of evaluation to decide behavior
		if (eval == 1) 						// do avoid ghost
		{
			return doBehavior(game, 1);
		}
		else if (eval == -1) 				// do eat ghost
		{
			return doBehavior(game, -1);
		}
		else 								// do follow pills
		{
			return doBehavior(game, 0);
		}
	}

	// behavior types: 1 == avoid ghost, -1 == eat ghost, 0 == follow pills
	public int doBehavior(Game game, int behaviorType)
	{
		if (behaviorType == 1) // behavior: avoid ghost
		{
			int result = -1;

			// get nearest ghost and pathfind away from it
			int ghost_Target = game.getTarget(pacman, ghost_Pos, true, G.DM.EUCLID);
			if (game.getEuclideanDistance(ghost_Target, pacman) < 5) // check if nearest ghost is actually close enough to run
			{
				result = game.getNextPacManDir(ghost_Target, false, Game.DM.EUCLID); // run away, ghost is close by
			}
			else
			{
				result = doBehavior(game, 0); // do follow pills - but check for nearby power pills before returning
			}

			// check power pills
			int[] power = game.getPowerPillIndices();
			ArrayList<Integer> targets_ArrayList = new ArrayList<Integer>();
			for (int i = 0; i < power.length; i++)
			{
				if (game.checkPowerPill(i))
				{
					targets_ArrayList.add(power[i]);
				}
			}

			// convert to array
			int[] targets_Array = new int[targets_ArrayList.size()];
			for (int i = 0; i < targets_Array.length; i++)
			{
				targets_Array[i] = targets_ArrayList.get(i);
			}

			// get nearest power pill from array
			int pill_target = game.getTarget(pacman, targets_Array, true, G.DM.PATH);
			if (pill_target != -1 && game.getPathDistance(pacman, pill_target) < 2) // if close enough -- grab power pill instead of running away
			{
				result = game.getNextPacManDir(pill_target, true, Game.DM.PATH);
			}

			return result;
		}
		else if (behaviorType == -1) // behavior: eat ghost
		{
			int ghost_Target = -1;

			// iterate over ghosts
			ArrayList<Integer> targets_ArrayList = new ArrayList<Integer>();
			for (int i = 0; i < Game.NUM_GHOSTS; i++)
			{
				// check if edible and then record ghost location if so
				if (game.isEdible(i))
				{
					targets_ArrayList.add(ghost_Pos[i]);
				}
				else if (!game.isEdible(i) && game.getEuclideanDistance(ghost_Pos[i], pacman) < 10)
				{
					return doBehavior(game, 1); // if ghost is found that is not edible and is very close -- should not eat ghost. abandon this behavior and do avoid ghost instead
				}
			}

			// convert to array
			int[] targets_Array = new int[targets_ArrayList.size()];
			for (int i = 0; i < targets_Array.length; i++)
			{
				targets_Array[i] = targets_ArrayList.get(i);
			}

			// get nearest target from array
			ghost_Target = game.getTarget(pacman, targets_Array, true, G.DM.PATH);

			if (ghost_Target != -1) // if target was found
			{
				return game.getNextPacManDir(ghost_Target, true, Game.DM.PATH);
			}
			else
			{
				return doBehavior(game, 0); // do follow pills if no edible ghost was found
			}
		}
		else if (behaviorType == 0) // behavior: follow pills
		{
			int[] pills = game.getPillIndices();
			int[] powerPills = game.getPowerPillIndices();

			// create arrayList of possible pill and powerPill targets
			ArrayList<Integer> targets_ArrayList = new ArrayList<Integer>();
			for (int i = 0; i < pills.length; i++)
			{
				if (game.checkPill(i))
				{
					targets_ArrayList.add(pills[i]);
				}
			}
			for (int i = 0; i < powerPills.length; i++)
			{
				if (game.checkPowerPill(i))
				{
					targets_ArrayList.add(powerPills[i]);
				}
			}

			// convert to array
			int[] targets_Array = new int[targets_ArrayList.size()];
			for (int i = 0; i < targets_Array.length; i++)
			{
				targets_Array[i] = targets_ArrayList.get(i);
			}

			// get nearest pill -- accounting for obstacles
			int target = game.getTarget(pacman, targets_Array, true, G.DM.MANHATTEN);
			int result = game.getNextPacManDir(target, true, Game.DM.PATH);
			
			// check if result is powerPill - and make sure it is still available
			int index = game.getPowerPillIndex(target);
			if (index != -1 && game.checkPowerPill(index))
			{
				if (game.getEuclideanDistance(pacman, target) < 2) // get close to power pill
				{
					// ms pacman cannot sit still, but she can rapidly change direction to 'hover' in one spot, allowing her to wait for an opportune moment to consume power pill
					result = game.getReverse(result); // wait for one or more ghosts to show up and trigger fleeing behavior -- guaranteeing good targets for eat ghost behavior
				}
			}

			return result;
		}

		return -1; // should never hit this
	}
}