import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.wb.swt.SWTResourceManager;

import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;

public class CompilerDesign {

	protected Shell shlCompilerApplication;
	private Text Expression;
	private Text Directory;
	private Group grpAnalysis;
	private Text Translator;
	private Group grpDefinition;
	private Text Grammar;
	private Text Word;
	private Button btnStart;
	private Label Grammar_1;
	private Label Word_1;
	private Label Lexer_1;
	private Label Parser_1;
	private Label Translator_1;
	
	static JavaToC j2c;
	private Table Parser;
	private LR lr;
	private String[] columnNames = {"步骤", "状态栈", "符号栈" ,"输入串", "动作说明"};
	private Text Lexer;
	private Table ParsingProcedure;

	
	/**
	 * Launch the application.
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			CompilerDesign window = new CompilerDesign();
			window.open();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Open the window.
	 */
	public void open() {
		Display display = Display.getDefault();
		createContents();
		shlCompilerApplication.open();
		shlCompilerApplication.layout();
		while (!shlCompilerApplication.isDisposed()) {
			if (!display.readAndDispatch()) {
				display.sleep();
			}
		}
	}

	/**
	 * Create contents of the window.
	 */
	protected void createContents() {
		
		j2c = new JavaToC();
		lr = new LR();
		
		shlCompilerApplication = new Shell();
		shlCompilerApplication.setImage(SWTResourceManager.getImage(CompilerDesign.class, "/FClient.ico"));
		shlCompilerApplication.setMinimumSize(new Point(300, 56));
		shlCompilerApplication.setSize(1195, 809);
		shlCompilerApplication.setText("Compiler Application");
		GridLayout gl_shlCompilerApplication = new GridLayout(2, true);
		gl_shlCompilerApplication.verticalSpacing = 3;
		gl_shlCompilerApplication.marginWidth = 0;
		gl_shlCompilerApplication.horizontalSpacing = 2;
		shlCompilerApplication.setLayout(gl_shlCompilerApplication);
		
		Group grpTestExpression = new Group(shlCompilerApplication, SWT.NONE);
		GridData gd_grpTestExpression = new GridData(SWT.FILL, SWT.FILL, false, false, 1, 1);
		gd_grpTestExpression.widthHint = 404;
		grpTestExpression.setLayoutData(gd_grpTestExpression);
		GridLayout gl_grpTestExpression = new GridLayout(3, false);
		gl_grpTestExpression.verticalSpacing = 3;
		gl_grpTestExpression.marginHeight = 0;
		gl_grpTestExpression.marginWidth = 0;
		grpTestExpression.setLayout(gl_grpTestExpression);
		grpTestExpression.setText("Test Expression");
		
		Label lblNewLabel = new Label(grpTestExpression, SWT.NONE);
		GridData gd_lblNewLabel = new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1);
		gd_lblNewLabel.widthHint = 91;
		lblNewLabel.setLayoutData(gd_lblNewLabel);
		lblNewLabel.setText("Directory");
		
		Directory = new Text(grpTestExpression, SWT.BORDER);
		Directory.setText("src/arithmetic1.test");
		GridData gd_Directory = new GridData(SWT.FILL, SWT.FILL, false, false, 1, 1);
		gd_Directory.widthHint = 275;
		Directory.setLayoutData(gd_Directory);
		
		btnStart = new Button(grpTestExpression, SWT.NONE);
		btnStart.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseUp(MouseEvent e) {
			if(btnStart.getText() == "Start") {
					btnStart.setText("Restart");
					String expr = j2c.DLL_GET_EXPRESSION(Directory.getText().trim());
					Expression.setText(expr);
					/**
					 * 1.开始分析
					 */
					String lexingTuple = j2c.DLL_PARSER(expr.trim()).substring(1);
					if(lexingTuple != "err") {	
						Lexer.setText(lexingTuple.split("STOP")[0]);
						/**
						 * 2.词法分析无误，则进行语法分析
						 * 语法分析并显示其过程，判断输入串是否合法
						 * 转换一下！！！
						 */
						LR.inString = expr;
						System.out.println("expr="+expr);
						lr.CU();
						if(LR.err) {
							//语法有误
							MessageBox dialog = new MessageBox(shlCompilerApplication,SWT.OK|SWT.ICON_INFORMATION);
							dialog.setText("Grammar error");
							dialog.setMessage("Input expression invalid!");
							dialog.open();
							LR.err = false;
						}
						else {
							/*
							 * 3.语法分析无误，进行翻译
							 */
							for(int i = 0; i < LR.LRAnalysis.size(); i++) {
								TableItem item = new TableItem(ParsingProcedure, SWT.NONE);  
							    item.setText(LR.LRAnalysis.get(i));
							}
							//重新布局表格并显示
							for(int i = 0; i < ParsingProcedure.getColumnCount(); i++) {
								 ParsingProcedure.getColumn(i).pack();
							}
							Translator.setText(lexingTuple.split("STOP")[1]);
						}						
					}
					else {
						//用词出错
						MessageBox dialog = new MessageBox(shlCompilerApplication,SWT.OK|SWT.ICON_INFORMATION);
						dialog.setText("Wording error");
						dialog.setMessage("Input expression invalid!");
						dialog.open();
					}
				}
				else {
					btnStart.setText("Start");
					Lexer.setText("");
					Expression.setText("");
					Translator.setText("");
					ParsingProcedure.removeAll();
				}
			}
		});
		btnStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1));
		btnStart.setText("Start");
		
		Expression = new Text(grpTestExpression, SWT.BORDER | SWT.READ_ONLY | SWT.MULTI);
		GridData gd_Expression = new GridData(SWT.LEFT, SWT.FILL, false, false, 2, 1);
		gd_Expression.heightHint = 82;
		gd_Expression.widthHint = 393;
		Expression.setLayoutData(gd_Expression);
		
		grpAnalysis = new Group(shlCompilerApplication, SWT.NONE);
		GridLayout gl_grpAnalysis = new GridLayout(2, false);
		grpAnalysis.setLayout(gl_grpAnalysis);
		grpAnalysis.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 2));
		grpAnalysis.setText("Analysis");
		
		Lexer_1 = new Label(grpAnalysis, SWT.NONE);
		Lexer_1.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, true, false, 1, 1));
		Lexer_1.setText("Lexer");
		
		Translator_1 = new Label(grpAnalysis, SWT.NONE);
		Translator_1.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
		Translator_1.setText("Translator");
		
		Lexer = new Text(grpAnalysis, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
		Lexer.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		
		Translator = new Text(grpAnalysis, SWT.BORDER | SWT.V_SCROLL | SWT.MULTI);
		Translator.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		
		ParsingProcedure = new Table(grpAnalysis, SWT.BORDER | SWT.FULL_SELECTION);
		ParsingProcedure.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		ParsingProcedure.setHeaderVisible(true);
		ParsingProcedure.setLinesVisible(true);
		
		grpDefinition = new Group(shlCompilerApplication, SWT.NONE);
		GridData gd_grpDefinition = new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1);
		gd_grpDefinition.heightHint = 0;
		gd_grpDefinition.widthHint = 294;
		grpDefinition.setLayoutData(gd_grpDefinition);
		GridLayout gl_grpDefinition = new GridLayout(1, false);
		gl_grpDefinition.verticalSpacing = 7;
		gl_grpDefinition.marginHeight = 0;
		gl_grpDefinition.horizontalSpacing = 10;
		grpDefinition.setLayout(gl_grpDefinition);
		grpDefinition.setText("Definition");
		
		Word_1 = new Label(grpDefinition, SWT.NONE);
		Word_1.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, true, false, 1, 1));
		Word_1.setText("Word");
		
		Word = new Text(grpDefinition, SWT.BORDER | SWT.READ_ONLY | SWT.V_SCROLL | SWT.MULTI);
		GridData gd_Word = new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1);
		gd_Word.heightHint = 89;
		Word.setLayoutData(gd_Word);
		Word.setText("Letter: a-z, A-Z\n" + 
					"Digit: 0~9\n" + 
					"Operator: + - * / ( )");
		
		Grammar_1 = new Label(grpDefinition, SWT.NONE);
		Grammar_1.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, true, false, 1, 1));
		Grammar_1.setText("Grammar");
		
		Grammar = new Text(grpDefinition, SWT.BORDER | SWT.READ_ONLY | SWT.WRAP | SWT.V_SCROLL | SWT.MULTI);
		GridData gd_Grammar = new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1);
		gd_Grammar.heightHint = 78;
		Grammar.setLayoutData(gd_Grammar);
		Grammar.setText("E→E+T|E-T\n" + "E→T\n" + 
						"T→T*F|T/F\n" + "T→F\n" + "F→(E)|i");
		
		Parser_1 = new Label(grpDefinition, SWT.NONE);
		Parser_1.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, true, false, 1, 1));
		Parser_1.setText("Parser");
		
		Parser = new Table(grpDefinition, SWT.BORDER | SWT.FULL_SELECTION | SWT.MULTI);
		Parser.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		Parser.setHeaderVisible(true);
		Parser.setLinesVisible(true);
		
		//显示LR分析表
        for(int i = 0; i < LR.row.length; i++) {
            TableColumn tableColumn = new TableColumn(Parser, SWT.NONE);
            tableColumn.setText(LR.row[i].toString());
            tableColumn.setMoveable(true);
        }
		for(int i = 0; i < LR.tbl.length; i++) {
			TableItem item = new TableItem(Parser, SWT.NONE);  
		    item.setText(LR.tbl[i]);  
		}
		//重新布局表格并显示
		for(int i = 0; i < Parser.getColumnCount(); i++) {
		    Parser.getColumn(i).pack();
		}
		//设置语法分析过程表表头
        for(int i = 0; i < columnNames.length; i++) {
            TableColumn tableColumn = new TableColumn(ParsingProcedure, SWT.NONE);
            tableColumn.setText(columnNames[i]);
            tableColumn.setMoveable(true);
        }
	}
}