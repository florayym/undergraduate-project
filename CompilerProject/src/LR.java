import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Stack;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 * 规范LR分析法
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
	
	//防止死循环
	ArrayList<String> deadArray = new ArrayList<String>();
	Character deadC;
	//
	
	/**
	 * 无参构造函数
	 */
	public LR() {
		grammar = "E→E+T|E-T|T\n" + 
				"T→T*F|T/F|F\n" + 
				"F→(E)|i";
		getVtVn();
		getFirst();
		getFollow();
		getGO();
		LRAnalysisTbl();
	}
	/**
	 * 初步处理输入的文法
	 * 得到非终结符集vn和终结符集vt
	 */
	protected void getVtVn() {
		grammar = "S→" + grammar.charAt(0) + "\n" + grammar;
		String[] gramStr = grammar.split("\n");
		ArrayList<String> item;
		String[] tempStr;
		String[] cStr;
		char c;
		//同一个Vn对应一个横向的内层ArrayList
		//get every sentence
		for(int i = 0;i < gramStr.length;i++) {
			ArrayList<String> mapRight = new ArrayList<String>();
			tempStr = gramStr[i].split("→");
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
		vtNew.remove('ε');
		vnNew.addAll(vn);
		vnNew.remove(vnNew.indexOf('S'));
	}
	/**
	 * 老方法构造FIRST(α)
	 */
	protected void getFirst() {
		ArrayList<String> itemArray;
		HashSet<Character> nFirst;
		Character n, e;
		for(int i = 0;i < vn.size();i++) {
			//control:X→Y1Y2…YiYi+1…Yk
			n = vn.get(i);
			nFirst = new HashSet<Character>();
			first.put(n, nFirst);
			itemArray =  gramMap.get(n);
			deadArray = itemArray;//dead
			for(String itemStr : itemArray) {
				for(int j = 0;j < itemStr.length();j++) {
					e = itemStr.charAt(j);
					//根据定义递归Yi，添加成功则break，否则寻找Yi+1
					if(iterFirst(e, nFirst)) break;
				}
			}
		}
	}
	/**
	 * 递归计算FIRST(α)函数
	 * @param e
	 * @param nFirst
	 * @return boolean
	 */
	protected boolean iterFirst(Character e, HashSet<Character> nFirst) {
		if(e == 'ε' || vt.contains(e)) {
			nFirst.add(e);
			if(e == 'ε') return false;
			return true;
		}		
		ArrayList<String> iterArray = gramMap.get(e);
		//由于已经合并，所以第一次一样，以后必定一样，会陷入死循环
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
		return !nFirst.contains('ε');
	}
	/**
	 * 构造FOLLOW(α)
	 * 假定将E作为确定的文法开始符号
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
							tempHSet.remove('ε');
							nFollow.addAll(tempHSet);
						}
						else nFollow.add(pc);
					}
				}
			}
		}
		//No.3
		//凡是通过步骤3添加的，都要注意最后保证FOLLOW添加完整
		//开始的问题就是策略正确，但由于不定的添加顺序，会导致添加不完整
		for(int i = 0;i < vn.size();i++) {
			n = vn.get(i);
			nFollow = new HashSet<Character>(follow.get(n));
			follow.replace(n, nFollow);
			for(ArrayList<String> itemStr : itemArray) {
				itemStrIndex = new ArrayList<Integer>(findVn(n, itemStr));
				for(int j = 0;j < itemStrIndex.size();j++) {
					sEnd = itemStr.get(itemStrIndex.get(j));
					if(sEnd.endsWith(n.toString()) || firstε(sEnd.substring(sEnd.indexOf(n.toString()) + 1))) {
						//找到产生式左部（即通过右部完整字串内容反过来寻找TreeMap中的key）
						ArrayList<String> al;
						for(int k = 0;k < vn.size();k++) {
							key = vn.get(k);
							al = new ArrayList<String>(gramMap.get(key));
							if(al.contains(sEnd)) {
								//问题：要解决“实际传递的应该是值，而不是地址”的问题
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
	 * 在一个ArrayList<String>的范围内判断一个非终结符哪一个产生式的右部出现
	 * （多个地方出现？）
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
	 * B后面紧跟着的β串可推出ε
	 * @param firstβ
	 * @return
	 */
	protected boolean firstε(String firstβ) {
		Character c;
		boolean endε = true;
		for(int i = 0;i < firstβ.length();i++) {
			c = firstβ.charAt(i);
			if(vt.contains(c) || !first.get(c).contains('ε')) {
				//进来的如果是vt，则要加入follow，若是后者为真，则加FIRST集中所有元素
				endε = false;
				break;
			}
		}
		return endε;
	}
	/**
	 * 联系各个项目集规范族
	 */
	protected void getGO() {
		HashSet<ArrayList<String>> pairSet = new HashSet<ArrayList<String>>();
		ArrayList<String> pair = new ArrayList<String>();
		//Initialize I0
		pairSet.add(pair);
		pair.add(0, "S");
		pair.add(1, ".E");
		//compute ε-CLOUSURE(J) to complete I0
		pairSet = CLOSURE_J(pairSet);
		status.put(0, pairSet);
		//snum记录现在进行到哪个status了
		int currentStatus = 0;
		/**
		 * 1.当前snum是否都被识别
		 * 是，为空：则snum++，取下一itemSet
		 * 否，不为空：则继续
		 */
		while(isNotOver(currentStatus)) {
			//对每一个状态，都有一个对应的closureMap
			HashMap<Character, HashSet<ArrayList<String>>> closureMap = new HashMap<Character, HashSet<ArrayList<String>>>();
			//对每一个a（下一字符），都有一个对应的closureItem
			HashSet<ArrayList<String>> closureItem;
			while(true) {
				//get next character
				pairSet = status.get(currentStatus);
				Iterator<ArrayList<String>> iterator = pairSet.iterator();
				while(iterator.hasNext()) {
					pair = new ArrayList<String>(iterator.next());
					String str = pair.get(1);
					int dot = str.indexOf(".");
					if(!pair.get(1).endsWith(".") && !pair.get(1).equals(".ε")) {
						//Map中value的更新要注意带上原有的（replace）
						char next = str.charAt(dot + 1);
						//有头绪了，因为closureMap.get(a)第一次是啥都没有。。。
						closureItem = new HashSet<ArrayList<String>>();
						if(closureMap.keySet().contains(next)) {
							closureItem.addAll(closureMap.get(next));
							closureMap.replace(next, closureItem);
						}
						else 
							closureMap.put(next, closureItem);
						//更新pairAL，改变.的位置	
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
			//计算go函数，并接着求ε-CLOUSURE(J)集，加入新的状态
			Iterator<Character> iterator = closureMap.keySet().iterator();
			while(iterator.hasNext()) {
				//因为Ix是确定的，对每一个a都有一个HashMap<HashMap<Integer, Character>, Integer> go函数，都要调用一次ε-CLOUSURE(J)函数
				char a = iterator.next();
				//不断加入新的go，但是go的中点状态是新的还是旧的，仍需进一步判断
				HashMap<Integer, Character> statusGo = new HashMap<Integer, Character>();
				statusGo.put(currentStatus, a);
				//先判断：用closureSetElement保存已经用过的closure原始元素集，可以判断closureMap.get(a)是否已经被用过，没用过就新建终点状态，并求新的ε-CLOUSURE(J)，直接加入新的nextStatus中
				if(closureSetElement.add(closureMap.get(a))) {
					int nextStatus = status.size();
					closureSetNumber.put(closureMap.get(a), nextStatus);
					go.put(statusGo, nextStatus);
					status.put(nextStatus, CLOSURE_J(closureMap.get(a)));
				}
				else 
					go.put(statusGo, closureSetNumber.get(closureMap.get(a)));	//加过的，则关联旧状态
			}
			currentStatus++;
		}
	}
	/**
	 * 判断一个状态是不是所有go都被识别了，也就是说这个传入的int对应的状态是不是终结了
	 * @param snum
	 * @param status
	 * @return
	 */
	protected boolean isNotOver(int currentStatus) {
		for(int i = currentStatus;i < status.size();i++) {
			ArrayList<ArrayList<String>> pairAL = new ArrayList<ArrayList<String>>(status.get(i));
			for(int j = 0;j < pairAL.size();j++)
				if(!pairAL.get(j).get(1).endsWith(".") && !pairAL.get(j).get(1).equals(".ε"))
					return true;
		}
		return false;
	}
	/**
	 * 递归嵌套求ε-CLOUSURE(J)
	 * J:pair array list
	 * @param J
	 * @return ArrayList<ArrayList<String>>
	 */
	protected HashSet<ArrayList<String>> CLOSURE_J(HashSet<ArrayList<String>> J) {
		//Jnext包含全部传进来的J pair HashSet
		HashSet<ArrayList<String>> Jnext = new HashSet<ArrayList<String>>(J);
		Iterator<ArrayList<String>> iterator = J.iterator();
		while(iterator.hasNext()) {
			String str = iterator.next().get(1);
			if(!str.endsWith(".") && !str.equals(".ε")) {
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
	 * 扩展ε-CLOUSURE(J)，.在首位
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
			if(!str.endsWith(".") && !str.equals(".ε") && vn.contains(str.charAt(1))) {
				if(str.charAt(1) != deadC) {
					deadC = str.charAt(1);
					Fnext.addAll(CLOSURE_F(deadC));
				}
			}
		}
		return Fnext;
	}
	/**
	 * 生成LR分析表的总控函数
	 */
	protected void LRAnalysisTbl() {
		//分析表架构及表头
		row = computeColumnNames();
		tbl = new String[status.size() + 1][row.length];
		//是否可能终止
		ArrayList<String> accPair = new ArrayList<String>();
		accPair.add("S");
		accPair.add("E.");
		HashSet<Character> rchar = null;
		int rnum = 0;
		//填充表格
		HashMap<Integer, Character> match;
		for(int i = 0;i < status.size();i++) {
			tbl[i][0] = String.valueOf(i);
			for(int j = 1;j <= vtNew.size();j++) {
				char a = String.valueOf(row[j]).charAt(0);
				match = new HashMap<Integer, Character>();
				match.put(i, a);
				//先求包不包含终结状态，并求其follow集
				ArrayList<String> terminatedPair = null;
				if(reduce(i) != null) {
					terminatedPair = new ArrayList<String>(reduce(i));
					char key = terminatedPair.get(0).charAt(0);
					rnum = findRuduceExprNum(terminatedPair);
					rchar = new HashSet<Character>(follow.get(key));		
				}
				//填s/r
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
	 * 填充表头
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
	 * 得到规约式
	 * @param currentStatus
	 * @return
	 */
	protected ArrayList<String> reduce(int currentStatus) {
		ArrayList<String> pair = null;
		Iterator<ArrayList<String>> iterator = status.get(currentStatus).iterator();
		while(iterator.hasNext()) {
			pair = new ArrayList<String>(iterator.next());
			if(pair.get(1).endsWith(".") || pair.get(1).equals(".ε"))
				return pair;
		}
		return null;
	}
	/**
	 * 求得要规约式子的序号
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
	 * LR分析总控程序
	 * ip，row，a，statusTop，statusStr，signTop，signStr实时更新
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
		LRAnalysis.add(new String[] {"0", statusStr, signStr, inString.substring(ip), "预备"});
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
				action += "  状态" + statusTop + "入栈";
			}
			else if(action.startsWith("r")) {
				//reduce
				ArrayList<String> reduceStr = new ArrayList<String>(reduce(statusTop));
				String temp = reduceStr.get(1);
				if(temp.endsWith(".")) {
					temp = temp.substring(0, temp.length() - 1);
				}
				else if(temp.equals(".ε")) {
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
				//状态栈更新
				statusTop = statusStack.peek();
				action = action + "：" + reduceStr.get(0) + "→" + temp + " 归约，GOTO(" + statusTop + ", ";
				next = Integer.valueOf(ACTION_GOTO(statusTop, signStack.peek().toString()));
				statusStack.push(next);
				statusTop = statusStack.peek();
				statusStr += statusTop;
				action += signStack.peek() + ")=" + next;
			}
			else if(action.equals("acc")) {
				action = "ACC：分析成功！";
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
	 * 查表获得动作指令
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
	 * 打印测试
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
