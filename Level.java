import java.util.*;
import java.util.stream.*;
import java.awt.*;
import javax.swing.*;

class Level{
	private static final int DRAW_WIDTH = 2000;
	private static final int DRAW_HEIGHT = 1000;
	
	//total number of r-differential posets found at each rank
	private static long[] numFound;
	
	private static long startTime;
	
	//number of nodes in top and bottom ranks
	private int nTop;
	private int nBottom;
	
	//description of cover relationships between top and bottom.
	//covers.get(i) contains Set of all elements in top that cover item i from bottom
	private ArrayList<HashSet<Integer>> covers;
	
	//rank of top
	public final int rank;
	
	//r in r-differential definition
	public static final int r = 1;
	
	//extensions of this.  empty set if has not been extended yet
	private HashSet<Level> children;
	
	//level that preceded this level
	private Level parent;
	
	//true if already extended
	private boolean hasBeenExtended = false;
	
	//if i<j and i and j from top cover common vertex from bottom, then j is in k2s.get(i)
	private ArrayList<HashSet<Integer>> k2s;
	
	//set of all sets of vertices of complete subgraphs with at least 3 vertices
	private ArrayList<HashSet<Integer>> k3plus;
	
	//array containing the upDegrees of vertices in top (same as down degree + r)
	private int[] upDegree;
	
	private static long addMissingK2sCalls = 0;
	private static long recursiveBuildLevelsCalls = 0;
	private static long attemptBuildChildCalls = 0;
	
/*	public boolean equals(Level that){
		if(nTop != that.nTop)
			return false;
		
		//find the down degrees of each top level
		int[] thisDownDegrees = new int[nTop];
		int[] thatDownDegrees = new int[that.nTop];
		
		for(int i=0; i<nBottom; i++)
			for(int j : covers.get(i))
				thisDownDegrees[j]++;
		
		for(int i=0; i<that.nBottom; i++)
			for(int j : that.covers.get(i))
				thatDownDegrees[j]++;

		//check if down degrees match
		for(int i=0; i<nTop; i++)
			if(thisDownDegrees[i] != thatDownDegrees[j])
				return false;
		
		//look for a possible permutation of the top
	}*/

	private void recursivek3plus(HashSet<Integer> currSet, int next){
		if(next == nTop){
			if(currSet.size() > 2)
				k3plus.add(currSet);
			return;
		}
		
		// find all sets without next
		HashSet<Integer> newCurrSet = new HashSet<Integer>(currSet);
		recursivek3plus(newCurrSet,next+1);

		//check if next can be added to currSet
		if(currSet.size() !=0)
			for(int i : currSet)
				if(!k2s.get(i).contains(next))
					return;
			
		//find all sets with next if next is addable
		currSet.add(next);
		recursivek3plus(currSet, next+1);

		return;
	}

	private int addMissingK2s(HashSet<Integer> currSet, ArrayList<HashSet<Integer>> possibleCovers, int possibleNTop){
		addMissingK2sCalls++;
		//add in missing k2's
		ArrayList<HashSet<Integer>> k2ToAdd = new ArrayList<>();
		for(HashSet<Integer> hS : k2s)
			k2ToAdd.add(new HashSet<Integer>(hS));
		
		for(int i : currSet)
			for(int j : k3plus.get(i))
				for(int k : k3plus.get(i))
					if(j < k)
						k2ToAdd.get(j).remove(k);
					
		for(int i=0; i<nTop; i++)
			for(int j: k2ToAdd.get(i)){
				possibleCovers.get(i).add(possibleNTop);
				possibleCovers.get(j).add(possibleNTop);
				possibleNTop++;
			}

		return possibleNTop;
	}
	
	private void attemptBuildChild(HashSet<Integer> currSet){
		attemptBuildChildCalls++;
/*		System.out.println("======================");
		System.out.print("currSet is: ");
		for(int i : currSet)
			System.out.print(i + ", ");
		System.out.println();*/

		int possibleNTop = 0;
		ArrayList<HashSet<Integer>> possibleCovers = new ArrayList<>();
		for(int i=0; i<nTop; i++)
			possibleCovers.add(new HashSet<Integer>());
		for(int i : currSet){
			for(int j : k3plus.get(i))
				possibleCovers.get(j).add(possibleNTop);
			possibleNTop++;
		}
			
		//add in missing k2's
		possibleNTop = addMissingK2s(currSet, possibleCovers, possibleNTop);
			
		//check if upDegree was violated
		boolean legal = true;
		for(int i=0; i<nTop && legal; i++){
			int gap = upDegree[i] - possibleCovers.get(i).size();
			if(gap < 0)
				legal = false;
			else
				for(int j=0; j<gap; j++){
					possibleCovers.get(i).add(possibleNTop);
					possibleNTop++;
				}
		}		
/*			System.out.print("currSet of: ");
		for(int i : currSet)
			System.out.print(i + ", ");
		System.out.println(" legal is: " + legal);*/

		if(!legal)
			return;
		
		numFound[rank+1]++;
		if(numFound[rank+1]%1000 == 0){
			System.out.println("found " + numFound[rank+1] + " at rank " + (rank+1) + " in " + (System.currentTimeMillis()-startTime)/1000.0 + " seconds so far");
			System.out.println("calls to addMissingK2s: " + addMissingK2sCalls);
			System.out.println("calls to recursiveBuildLevels: " + recursiveBuildLevelsCalls);
			System.out.println("calls to attemptBuildChild: " + attemptBuildChildCalls);
			System.out.println("==================================================");
		}
		
		Level child = new Level(rank+1);
		child.nBottom = nTop;
		child.nTop = possibleNTop;
		child.covers = possibleCovers;
		
		children.add(child);
		child.parent = this;
		for(int i=0; i<child.nTop; i++)
			child.k2s.add(new HashSet<Integer>());

		child.upDegree = new int[child.nTop];
		for(int i=0; i<child.nTop; i++)
			child.upDegree[i] = r;

		for(HashSet<Integer> hS : child.covers)
			for(int i : hS)
				child.upDegree[i]++;
			
		//reorder nodes of child for prettiness of drawings.
		//find left node
		
		int leftNode = 0;
		for(int i : child.covers.get(0))
			if(child.upDegree[i] == r+1){
				leftNode = i;
				break;
			}
			
		int[] newOrder = new int[child.nTop];
		for(int i=0; i<child.nTop; i++)
			newOrder[i] = -1;
		int ind = 1;
		newOrder[leftNode] = 0;
		for(int i=0; i<child.nBottom; i++){
			for(int j : child.covers.get(i))
				if(newOrder[j] == -1)
					newOrder[j] = ind++;
		}

		//update upDegree to reflect newOrder
		int[] newUpDegree = new int[child.nTop];
		for(int i=0; i<child.nTop; i++)
			newUpDegree[newOrder[i]] = child.upDegree[i];
		for(int i=0; i<child.nTop; i++)
			child.upDegree[i] = newUpDegree[i];
		
		//swap covers to reflect newOrder
		for(int i=0; i<child.nBottom; i++){
			HashSet<Integer> newHS = new HashSet<Integer>();
			for(int j : child.covers.get(i))
				newHS.add(newOrder[j]);
			child.covers.set(i, newHS);
		}
		
		return;
	}
	
	//recursively adds/not adds elements of k3plus when next hits ksplus.size(), tries to build child Level
	private void recursiveBuildLevels(HashSet<Integer> currSet, int next){
		recursiveBuildLevelsCalls++;
		if(next == k3plus.size()){
			//try to build child Level using all elements of currSet
			attemptBuildChild(currSet);

			return;
		}

		HashSet<Integer> newCurrSet = new HashSet<Integer>(currSet);
		recursiveBuildLevels(newCurrSet, next+1);
		
		//check if k3plus.get(i) can be added (looking for pair of vertices already in element of currSet)
		if(currSet.size() != 0)
			for(int i : k3plus.get(next))
				for(int j : k3plus.get(next))
					if(i < j)
						for(int k : currSet)
							if(k3plus.get(k).contains(i) && k3plus.get(k).contains(j))
								return;
		currSet.add(next);
		recursiveBuildLevels(currSet,next+1);
		
		return;
	}
	
	public HashSet<Level> extend(){
/*		output();
		System.out.print("upDegrees: ");
		for(int i=0; i<nTop; i++)
			System.out.print(upDegree[i] + ", ");
		System.out.println();*/
		
		if(hasBeenExtended)
			return children;
		else
			hasBeenExtended = true;
		
		//build ArrayList of all k2s
		for(HashSet<Integer> hS : covers)
			for(int a : hS)
				for(int b : hS)
					if(a < b)
						k2s.get(a).add(b);

/*		for(int i=0; i<nTop; i++)
			for(int j : k2s.get(i))
				System.out.println(i + " and " + j + " are adjacent");
		System.out.println("++++++++++++++++++++++++++");*/
		
		//build set of all k3plus's
		recursivek3plus(new HashSet<Integer>(),0);
		
/*		for(HashSet<Integer> hS : k3plus){
			System.out.print("Set of: ");
			for(int i : hS)
				System.out.print(i + ", ");
			System.out.println();
		}*/
		
		//recursviely go through set of all k3plus's and convert to appropriate covers
		//1. use recursive helper function to find all combinations of k3plus that don't overlap
		//		2. for each combination fill in with missing k2s and check if up degree restrictions are met
		//		3. build new Level if up degree restrictions are met
		
		//1. use recursive helper function to find all combinations of k3plus that don't overlap
		recursiveBuildLevels(new HashSet<Integer>(), 0);
		
		return children;
	}
	
	//outputs the level in human-readable form
	public void output(){
		System.out.println("Rank: " + rank);
		for(int i=0; i<covers.size(); i++)
			for(Integer j : covers.get(i)){
				System.out.println("(" + i + "," + j + ")");
			}
		if(parent!=null)
			parent.output();
		else
			System.out.println("************************************");
		return;
	}
	
	public Level(int rank){
		this.rank = rank;
		
		children = new HashSet<Level>();
		
		nBottom = 0;
		nTop = 1;
		covers = new ArrayList<HashSet<Integer>>();
		parent = null;
		
		k2s = new ArrayList<HashSet<Integer>>(nTop);
		k3plus = new ArrayList<HashSet<Integer>>();
		for(int i=0; i<nTop; i++)
			k2s.add(new HashSet<Integer>());
		
		upDegree = new int[nTop];
		for(int i=0; i<nTop; i++)
			upDegree[i] = r;
		
		for(HashSet<Integer> hS : covers)
			for(int i : hS)
				upDegree[i]++;
	}
	
	public Level(){
		this(0);
	}
	
	public static void main(String[] args){
		int maxRank = Integer.parseInt(args[0]);
		startTime = System.currentTimeMillis();
		numFound = new long[maxRank+1];
		numFound[0] = 1;
		
		Level l = new Level();
		
		HashSet<Level> extendSet = new HashSet<Level>();
		extendSet.add(l);
	
		for(int i=0; i<maxRank; i++){
			extendSet = extendSet.stream()
								.flatMap(lev -> lev.extend().stream())
								.collect(Collectors.toCollection(HashSet<Level>::new));
		}
		
		int count = 0;
		for(Level lev : extendSet){
			if(count++ == 0){
				lev.output();
				new DrawPosetFrame(lev);
//				dP.setSize(300,300);
//				dP.setVisible(true);
			}
		}
		
		for(int i=0; i<=maxRank; i++)
			System.out.println("number " + r + "-differential posets up to rank " + i + " is: " + numFound[i]);
		
		System.out.println("Total time: " + (System.currentTimeMillis()-startTime)/1000.0);
		System.out.println("calls to addMissingK2s: " + addMissingK2sCalls);
		System.out.println("calls to recursiveBuildLevels: " + recursiveBuildLevelsCalls);
		System.out.println("calls to attemptBuildChild: " + attemptBuildChildCalls);
		

	}
	
	public static class DrawPosetPanel extends JPanel{
		Level baseLevel;
		public DrawPosetPanel(Level lev){
			baseLevel = lev;
		}
		
		public DrawPosetPanel(){
			super();
		}
		
		public void paintComponent(Graphics g){
			super.paintComponent(g);
			Level p = baseLevel;
			while(p != null){
				int[] x1 = new int[p.nBottom];
				int[] y1 = new int[p.nBottom];
				for(int i=0; i<p.nBottom; i++){
					x1[i] = DRAW_WIDTH/2 - 50*(p.nBottom-1)/2 + i*50;
					y1[i] = DRAW_HEIGHT - p.rank*100;
				}
				
				int[] x2 = new int[p.nTop];
				int[] y2 = new int[p.nTop];
				for(int i=0; i<p.nTop; i++){
					x2[i] = DRAW_WIDTH/2 - 50*(p.nTop-1)/2 + i*50;
					y2[i] = DRAW_HEIGHT - (p.rank+1)*100;
				}
				
				for(int i=0; i<p.nBottom; i++)
					g.fillOval(x1[i],y1[i],10,10);
				for(int i=0; i<p.nTop; i++)
					g.fillOval(x2[i],y2[i],10,10);
				for(int i=0; i<p.covers.size(); i++)
					for(int j : p.covers.get(i))
						g.drawLine(x1[i]+5,y1[i]+5,x2[j]+5,y2[j]+5);
				p = p.parent;
			}
		}
	}
	
	public static class DrawPosetFrame extends JFrame{
		public DrawPosetFrame(){
			super();
			setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			setSize(DRAW_WIDTH,DRAW_HEIGHT);
			add(new DrawPosetPanel());
//			pack();
			setVisible(true);
		}
		
		public DrawPosetFrame(Level lev){
			super();
			setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			setSize(DRAW_WIDTH,DRAW_HEIGHT);
			add(new DrawPosetPanel(lev));
//			pack();
			setVisible(true);
		}
	}
}