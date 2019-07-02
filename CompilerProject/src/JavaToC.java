
public class JavaToC {
	
	/**
	 * @author Flora
	 * Declare c dll functions.
	 * Compile command: javac -h . JavaFileName.java
	 * 
	 * @DLL_Initialization
	 * 
	 * @DLL_GET_EXPRESSION
	 * 
	 * @DLL_LEXER
	 * 
	 */
	public native String DLL_GET_EXPRESSION(String filePath);
	public native String DLL_PARSER(String expression);

    static {
        System.loadLibrary("CompilerDll");
    }
}
