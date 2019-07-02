import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Stack;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 * �淶LR������
 * 
 * @author YuQifei
 */	
public class LR {
	
	protected static String grammar = null;
	protected static String inString = null;
	protected static boolean err = false;
	protected static String[][] tbl;
	protected static Character[] row;
	protected static ArrayList<String[]> LRAnalysis = new ArrayList<String[]>(); 
	private static ArrayList<ArrayList<String>> LRItem = new ArrayList<ArrayList<String>>();
	private static TreeMap<Character, ArrayList<String>> gramMap = new TreeMap<Character, ArrayList<String>>();
	private static ArrayList<Character> vn = new ArrayList<Character>(); /*****/
	private static TreeSet<Character> vt = new TreeSet<Character>();
	private static HashMap<Character, HashSet<Character>> first = new HashMap<Character, HashSet<Character>>();
	private static HashMap<Character, HashSet<Character>> follow = new HashMap<Character, HashSet<Character>>();
	private static HashMap<HashMap<Integer, Character>, Integer> go = new HashMap<HashMap<Integer, Character>, Integer>();
	private static HashMap<Integer, HashSet<ArrayList<String>>> status = new HashMap<Integer, HashSet<ArrayList<String>>>();
	private static HashSet<HashSet<ArrayList<String>>> closureSetElement = new HashSet<HashSet<ArrayList<String>>>();
	private static HashMap<HashSet<ArrayList<String>>, Integer> closureSetNumber = new HashMap<HashSet<ArrayList<String>>, Integer>();
	private static TreeSet<Character> vtNew = new TreeSet<Character>();
	private static ArrayList<Character> vnNew = new ArrayList<Character>();
	
	//��ֹ��ѭ��
	ArrayList<String> deadArray = new ArrayList<String>();
	Character deadC;
	//
	
	/**
	 * �޲ι��캯��
	 */
	public LR() {
		grammar = "E��E+T|E-T|T\n" + 
				"T��T*F|T/F|F\n" + 
				"F��(E)|i";
		getVtVn();
		getFirst();
		getFollow();
		getGO();
		LRAnalysisTbl();
	}
	/**
	 * ��������������ķ�
	 * �õ����ս����vn���ս����vt
	 */
	protected void getVtVn() {
		grammar = "S��" + grammar.charAt(0) + "\n" + grammar;
		String[] gramStr = grammar.split("\n");
		ArrayList<String> item;
		String[] tempStr;
		String[] cStr;
		char c;
		//ͬһ��Vn��Ӧһ��������ڲ�ArrayList
		//get every sentence
		for(int i = 0;i < gramStr.length;i++) {
			ArrayList<String> mapRight = new ArrayList<String>();
			tempStr = gramStr[i].split("��");
			c = tempStr[0].charAt(0);
			cStr = tempStr[1].split("\\|");
			vn.add(c);
			gramMap.put(c, mapRight);
			for(int j = 0;j < cStr.length;j++) {
				item = new ArrayList<String>();
				LRItem.add(item);
				item.add(String.valueOf(c));
				item.add(cStr[j]);
				mapRight.add(cStr[j]);
				//build vt set
				for(int k = 0;k < cStr[j].length() + 1;k++) 
					if(k < cStr[j].length())
						vt.add(cStr[j].charAt(k));
			}
		}
		vt.removeAll(vn);
		//ready
		vtNew.addAll(vt);
		vtNew.remove('��');
		vnNew.addAll(vn);
		vnNew.remove(vnNew.indexOf('S'));
	}
	/**
	 * �Ϸ�������FIRST(��)
	 */
	protected void getFirst() {
		ArrayList<String> itemArray;
		HashSet<Character> nFirst;
		Character n, e;
		for(int i = 0;i < vn.size();i++) {
			//control:X��Y1Y2��YiYi+1��Yk
			n = vn.get(i);
			nFirst = new HashSet<Character>();
			first.put(n, nFirst);
			itemArray =  gramMap.get(n);
			deadArray = itemArray;//dead
			for(String itemStr : itemArray) {
				for(int j = 0;j < itemStr.length();j++) {
					e = itemStr.charAt(j);
					//���ݶ���ݹ�Yi����ӳɹ���break������Ѱ��Yi+1
					if(iterFirst(e, nFirst)) break;
				}
			}
		}
	}
	/**
	 * �ݹ����FIRST(��)����
	 * @param e
	 * @param nFirst
	 * @return boolean
	 */
	protected boolean iterFirst(Character e, HashSet<Character> nFirst) {
		if(e == '��' || vt.contains(e)) {
			nFirst.add(e);
			if(e == '��') return false;
			return true;
		}		
		ArrayList<String> iterArray = gramMap.get(e);
		//�����Ѿ��ϲ������Ե�һ��һ�����Ժ�ض�һ������������ѭ��
		if(iterArray.equals(deadArray))	{
			return true;
		}
		else {
			deadArray = iterArray;
		}
		for(String tempStr : iterArray) {
			//Only watch the first character of Yi
			Character f = tempStr.charAt(0);
			iterFirst(f, nFirst);
		}
		return !nFirst.contains('��');
	}
	/**
	 * ����FOLLOW(��)
	 * �ٶ���E��Ϊȷ�����ķ���ʼ����
	 */
	protected void getFollow() {
		Collection<ArrayList<String>> itemArray = gramMap.values();
		HashSet<Character> nFollow, tempHSet;
		ArrayList<Integer> itemStrIndex;
		Character n, pc, key;
		String sEnd = null;
		//No.1&2
		Iterator<Character> iterator = vn.iterator();
		while(iterator.hasNext()) {
			n = iterator.next();
			nFollow = new HashSet<Character>();
			follow.put(n, nFollow);
			if(n.equals('S'))
				nFollow.add('#');
			for(ArrayList<String> itemStr : itemArray) {
				itemStrIndex = new ArrayList<Integer>(findVn(n, itemStr));
				for(int i = 0;i < itemStrIndex.size();i++) {
					sEnd = itemStr.get(itemStrIndex.get(i));
					if(!sEnd.endsWith(n.toString())) {
						pc = sEnd.charAt(sEnd.indexOf(n.toString()) + 1);
						if(vn.contains(pc)) {
							tempHSet = new HashSet<Character>(first.get(pc));
							tempHSet.remove('��');
							nFollow.addAll(tempHSet);
						}
						else nFollow.add(pc);
					}
				}
			}
		}
		//No.3
		//����ͨ������3��ӵģ���Ҫע�����֤FOLLOW�������
		//��ʼ��������ǲ�����ȷ�������ڲ��������˳�򣬻ᵼ����Ӳ�����
		for(int i = 0;i < vn.size();i++) {
			n = vn.get(i);
			nFollow = new HashSet<Character>(follow.get(n));
			follow.replace(n, nFollow);
			for(ArrayList<String> itemStr : itemArray) {
				itemStrIndex = new ArrayList<Integer>(findVn(n, itemStr));
				for(int j = 0;j < itemStrIndex.size();j++) {
					sEnd = itemStr.get(itemStrIndex.get(j));
					if(sEnd.endsWith(n.toString()) || first��(sEnd.substring(sEnd.indexOf(n.toString()) + 1))) {
						//�ҵ�����ʽ�󲿣���ͨ���Ҳ������ִ����ݷ�����Ѱ��TreeMap�е�key��
						ArrayList<String> al;
						for(int k = 0;k < vn.size();k++) {
							key = vn.get(k);
							al = new ArrayList<String>(gramMap.get(key));
							if(al.contains(sEnd)) {
								//���⣺Ҫ�����ʵ�ʴ��ݵ�Ӧ����ֵ�������ǵ�ַ��������
								if(!key.equals(n)) nFollow.addAll(follow.get(key));
								break;
							}
						}
					}
				}
			}
		}
	}
	/**
	 * ��һ��ArrayList<String>�ķ�Χ���ж�һ�����ս����һ������ʽ���Ҳ�����
	 * ������ط����֣���
	 * @param n
	 * @param itemStr
	 * @return index of matched String in the ArrayList<String>
	 */
	protected ArrayList<Integer> findVn(Character n, ArrayList<String> itemStr) {
		ArrayList<Integer> iAL = new ArrayList<Integer>();
		for(int i = 0;i < itemStr.size();i++) {
			if(itemStr.get(i).contains(n.toString())) iAL.add(Integer.valueOf(i));
		}
		return iAL;
	}
	/**
	 * B��������ŵĦ´����Ƴ���
	 * @param first��
	 * @return
	 */
	protected boolean first��(String first��) {
		Character c;
		boolean end�� = true;
		for(int i = 0;i < first��.length();i++) {
			c = first��.charAt(i);
			if(vt.contains(c) || !first.get(c).contains('��')) {
				//�����������vt����Ҫ����follow�����Ǻ���Ϊ�棬���FIRST��������Ԫ��
				end�� = false;
				break;
			}
		}
		return end��;
	}
	/**
	 * ��ϵ������Ŀ���淶��
	 */
	protected void getGO() {
		HashSet<ArrayList<String>> pairSet = new HashSet<ArrayList<String>>();
		ArrayList<String> pair = new ArrayList<String>();
		//Initialize I0
		pairSet.add(pair);
		pair.add(0, "S");
		pair.add(1, ".E");
		//compute ��-CLOUSURE(J) to complete I0
		pairSet = CLOSURE_J(pairSet);
		status.put(0, pairSet);
		//snum��¼���ڽ��е��ĸ�status��
		int currentStatus = 0;
		/**
		 * 1.��ǰsnum�Ƿ񶼱�ʶ��
		 * �ǣ�Ϊ�գ���snum++��ȡ��һitemSet
		 * �񣬲�Ϊ�գ������
		 */
		while(isNotOver(currentStatus)) {
			//��ÿһ��״̬������һ����Ӧ��closureMap
			HashMap<Character, HashSet<ArrayList<String>>> closureMap = new HashMap<Character, HashSet<ArrayList<String>>>();
			//��ÿһ��a����һ�ַ���������һ����Ӧ��closureItem
			HashSet<ArrayList<String>> closureItem;
			while(true) {
				//get next character
				pairSet = status.get(currentStatus);
				Iterator<ArrayList<String>> iterator = pairSet.iterator();
				while(iterator.hasNext()) {
					pair = new ArrayList<String>(iterator.next());
					String str = pair.get(1);
					int dot = str.indexOf(".");
					if(!pair.get(1).endsWith(".") && !pair.get(1).equals(".��")) {
						//Map��value�ĸ���Ҫע�����ԭ�еģ�replace��
						char next = str.charAt(dot + 1);
						//��ͷ���ˣ���ΪclosureMap.get(a)��һ����ɶ��û�С�����
						closureItem = new HashSet<ArrayList<String>>();
						if(closureMap.keySet().contains(next)) {
							closureItem.addAll(closureMap.get(next));
							closureMap.replace(next, closureItem);
						}
						else 
							closureMap.put(next, closureItem);
						//����pairAL���ı�.��λ��	
						pair.remove(1);
						if(dot == str.length() - 2)
							str = str.substring(0, dot) + str.charAt(dot + 1) + ".";
						else 
							str = str.substring(0, dot) + str.charAt(dot + 1) + "." + str.substring(dot + 2);
						pair.add(1, str);
						closureItem.add(pair);
					}
				}
				if(closureMap.keySet().isEmpty()) currentStatus++;
				else break;
			}
			//����go���������������-CLOUSURE(J)���������µ�״̬
			Iterator<Character> iterator = closureMap.keySet().iterator();
			while(iterator.hasNext()) {
				//��ΪIx��ȷ���ģ���ÿһ��a����һ��HashMap<HashMap<Integer, Character>, Integer> go��������Ҫ����һ�Φ�-CLOUSURE(J)����
				char a = iterator.next();
				//���ϼ����µ�go������go���е�״̬���µĻ��Ǿɵģ������һ���ж�
				HashMap<Integer, Character> statusGo = new HashMap<Integer, Character>();
				statusGo.put(currentStatus, a);
				//���жϣ���closureSetElement�����Ѿ��ù���closureԭʼԪ�ؼ��������ж�closureMap.get(a)�Ƿ��Ѿ����ù���û�ù����½��յ�״̬�������µĦ�-CLOUSURE(J)��ֱ�Ӽ����µ�nextStatus��
				if(closureSetElement.add(closureMap.get(a))) {
					int nextStatus = status.size();
					closureSetNumber.put(closureMap.get(a), nextStatus);
					go.put(statusGo, nextStatus);
					status.put(nextStatus, CLOSURE_J(closureMap.get(a)));
				}
				else 
					go.put(statusGo, closureSetNumber.get(closureMap.get(a)));	//�ӹ��ģ��������״̬
			}
			currentStatus++;
		}
	}
	/**
	 * �ж�һ��״̬�ǲ�������go����ʶ���ˣ�Ҳ����˵��������int��Ӧ��״̬�ǲ����ս���
	 * @param snum
	 * @param status
	 * @return
	 */
	protected boolean isNotOver(int currentStatus) {
		for(int i = currentStatus;i < status.size();i++) {
			ArrayList<ArrayList<String>> pairAL = new ArrayList<ArrayList<String>>(status.get(i));
			for(int j = 0;j < pairAL.size();j++)
				if(!pairAL.get(j).get(1).endsWith(".") && !pairAL.get(j).get(1).equals(".��"))
					return true;
		}
		return false;
	}
	/**
	 * �ݹ�Ƕ�����-CLOUSURE(J)
	 * J:pair array list
	 * @param J
	 * @return ArrayList<ArrayList<String>>
	 */
	protected HashSet<ArrayList<String>> CLOSURE_J(HashSet<ArrayList<String>> J) {
		//Jnext����ȫ����������J pair HashSet
		HashSet<ArrayList<String>> Jnext = new HashSet<ArrayList<String>>(J);
		Iterator<ArrayList<String>> iterator = J.iterator();
		while(iterator.hasNext()) {
			String str = iterator.next().get(1);
			if(!str.endsWith(".") && !str.equals(".��")) {
				char next = str.charAt(str.indexOf('.') + 1);
				deadC = next;
				if(vn.contains(next)) {
					Jnext.addAll(CLOSURE_F(next));
				}
			}
		}
		return Jnext;
	}
	/**
	 * ��չ��-CLOUSURE(J)��.����λ
	 * @param F
	 * @return
	 */
	protected HashSet<ArrayList<String>> CLOSURE_F(char F) {
		HashSet<ArrayList<String>> Fnext = new HashSet<ArrayList<String>>();
		for(int i = 0;i < gramMap.get(F).size();i++) {
			ArrayList<String> pair = new ArrayList<String>();
			pair.add(String.valueOf(F));//pair 0
			pair.add("." + gramMap.get(F).get(i));//pair 1
			Fnext.add(pair);
			String str = pair.get(1);
			if(!str.endsWith(".") && !str.equals(".��") && vn.contains(str.charAt(1))) {
				if(str.charAt(1) != deadC) {
					deadC = str.charAt(1);
					Fnext.addAll(CLOSURE_F(deadC));
				}
			}
		}
		return Fnext;
	}
	/**
	 * ����LR��������ܿغ���
	 */
	protected void LRAnalysisTbl() {
		//������ܹ�����ͷ
		row = computeColumnNames();
		tbl = new String[status.size() + 1][row.length];
		//�Ƿ������ֹ
		ArrayList<String> accPair = new ArrayList<String>();
		accPair.add("S");
		accPair.add("E.");
		HashSet<Character> rchar = null;
		int rnum = 0;
		//�����
		HashMap<Integer, Character> match;
		for(int i = 0;i < status.size();i++) {
			tbl[i][0] = String.valueOf(i);
			for(int j = 1;j <= vtNew.size();j++) {
				char a = String.valueOf(row[j]).charAt(0);
				match = new HashMap<Integer, Character>();
				match.put(i, a);
				//������������ս�״̬��������follow��
				ArrayList<String> terminatedPair = null;
				if(reduce(i) != null) {
					terminatedPair = new ArrayList<String>(reduce(i));
					char key = terminatedPair.get(0).charAt(0);
					rnum = findRuduceExprNum(terminatedPair);
					rchar = new HashSet<Character>(follow.get(key));		
				}
				//��s/r
				if(go.containsKey(match))
					tbl[i][j] = "s" + go.get(match);
				else if((reduce(i) != null) && rchar.contains(a))
					tbl[i][j] = "r" + rnum;
			}
			if(status.get(i).contains(accPair))
				tbl[i][vtNew.size() + 1] = "acc";
			else if((reduce(i) != null) && rchar.contains('#')){
				tbl[i][vtNew.size() + 1] = "r" + rnum;
			}
			for(int k = 0;k < vnNew.size();k++) {
				match = new HashMap<Integer, Character>();
				match.put(i, vnNew.get(k));
				if(go.containsKey(match)) {
					tbl[i][vtNew.size() + 2 + k] = String.valueOf(go.get(match));
				}
			}
		}
	}
	/**
	 * ����ͷ
	 * @return
	 */
	protected static Character[] computeColumnNames() {
		Character[] row = new Character[vnNew.size() + vtNew.size() + 2];
		
		row[0] = ' ';
		int i = 1;
		
		Iterator<Character> iterator = vtNew.iterator();
		while(iterator.hasNext()) {
			row[i] = iterator.next();
			i++;
		}
		row[i++] = '#';
		for(int j = 0;j < vnNew.size();j++) {
			row[i + j] = vnNew.get(j);
		}
		return row;
	}
	/**
	 * �õ���Լʽ
	 * @param currentStatus
	 * @return
	 */
	protected ArrayList<String> reduce(int currentStatus) {
		ArrayList<String> pair = null;
		Iterator<ArrayList<String>> iterator = status.get(currentStatus).iterator();
		while(iterator.hasNext()) {
			pair = new ArrayList<String>(iterator.next());
			if(pair.get(1).endsWith(".") || pair.get(1).equals(".��"))
				return pair;
		}
		return null;
	}
	/**
	 * ���Ҫ��Լʽ�ӵ����
	 * @param tPair
	 * @return
	 */
	protected int findRuduceExprNum(ArrayList<String> tPair) {
		String expr = tPair.get(1);
		if(expr.endsWith("."))
			expr = expr.substring(0, expr.length() - 1);
		else 
			expr = expr.substring(1);
		for(int i = 1;i < LRItem.size();i++) {
			if(LRItem.get(i).get(0).equals(tPair.get(0)) && LRItem.get(i).get(1).equals(expr)) {
				return i;
			}
		}
		return -1;
	}
	
	protected int getForward(int i) {
		if((inString.charAt(i) >= 48) && (inString.charAt(i) <= 57)) {
			while((inString.charAt(i) >= 48) && (inString.charAt(i) <= 57)) {
				i++;
			}
		}
		else {
			i++;
		}
		return i;
	}
		
	/**
	 * LR�����ܿس���
	 * ip��row��a��statusTop��statusStr��signTop��signStrʵʱ����
	 */
	protected void CU() {
		int next;
		String action, statusStr = "0", signStr = "#";
		Stack<Integer> statusStack = new Stack<Integer>();
		Stack<Character> signStack = new Stack<Character>();
		statusStack.push(0);
		signStack.push('#');
		inString += '#';
		int ip = 0, row = 1;
		String a = inString.substring(ip, getForward(ip));
		int statusTop = statusStack.peek();
		LRAnalysis.add(new String[] {"0", statusStr, signStr, inString.substring(ip), "Ԥ��"});
		while(true) {
			action = ACTION_GOTO(statusTop, a);
			if(action.startsWith("s")) {
				next = action.charAt(1) - 48;
				if(action.length() == 3) {
						next = next * 10 + action.charAt(2) - 48;
				}
				action = "ACTION[" + statusTop + ", " + a + "]=" + action; 
				statusStack.push(Integer.valueOf(next));
				statusTop = statusStack.peek();
				statusStr += statusTop + "";
				if((inString.charAt(ip) >= 48) && (inString.charAt(ip) <= 57)) {
					signStack.push('i');
				}
				else {
					signStack.push(a.charAt(0));
				}
				signStr += a;
				ip = getForward(ip);
				a = inString.substring(ip, getForward(ip));
				action += "  ״̬" + statusTop + "��ջ";
			}
			else if(action.startsWith("r")) {
				//reduce
				ArrayList<String> reduceStr = new ArrayList<String>(reduce(statusTop));
				String temp = reduceStr.get(1);
				if(temp.endsWith(".")) {
					temp = temp.substring(0, temp.length() - 1);
				}
				else if(temp.equals(".��")) {
					temp = "";
				}
				for(int i = 0;i < temp.length();i++) {
					signStack.pop();
					if(statusStack.pop() > 9)
						statusStr = statusStr.substring(0,  statusStr.length() - 2);
					else 
						statusStr = statusStr.substring(0,  statusStr.length() - 1);
				}
				signStack.push(reduceStr.get(0).charAt(0));
				signStr = signStr.substring(0, signStr.length() - temp.length()) + signStack.peek();
				//״̬ջ����
				statusTop = statusStack.peek();
				action = action + "��" + reduceStr.get(0) + "��" + temp + " ��Լ��GOTO(" + statusTop + ", ";
				next = Integer.valueOf(ACTION_GOTO(statusTop, signStack.peek().toString()));
				statusStack.push(next);
				statusTop = statusStack.peek();
				statusStr += statusTop;
				action += signStack.peek() + ")=" + next;
			}
			else if(action.equals("acc")) {
				action = "ACC�������ɹ���";
				break;
			}
			else {
				action = "Failed";
				err = true;
				break;
			}
			LRAnalysis.add(new String[] {String.valueOf(row), statusStr, signStr, inString.substring(ip), action});
			row++;
		}
		LRAnalysis.add(new String[] {String.valueOf(row), statusStr, signStr, inString.substring(ip), action});
	}
	/**
	 * ����ö���ָ��
	 * @param sta
	 * @param a
	 * @return
	 */
	protected String ACTION_GOTO(int sta, String s) {
		String command = "error";
		char a;
		if((s.charAt(0) >= 48) && (s.charAt(0) <= 57)) {
			a = 'i';
		}
		else {
			a = s.charAt(0);
		}
		for(int i = 1;i < row.length;i++) {
			if(row[i].equals(a)) {
				command = String.valueOf(tbl[sta][i]);
				break;
			}
		}
		return command;
	}
	/**
	 * ��ӡ����
	 * @return
	 */
	protected void print() {
		for(int i = 0;i < LR.tbl.length;i++) {
			for(int j = 0;j < LR.row.length;j++) {
				System.out.print(LR.tbl[i][j] + " ");
			}
			System.out.println("\n");
		}
		System.out.println("\nLRAnalysis:\n" + LRAnalysis);
	}
}
