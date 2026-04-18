//
// Compilar con «csc /reference:System.IO.Compression.FileSystem.dll /reference:System.IO.Compression.dll /win32manifest:manifest.xml Entrega.cs»
//

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;

using System.Windows.Forms;

/*
 * Program to compress the submissions without unimportant information.
 */
class Submission {

	private const String SUBJECT_NAME = "Gráficos por Computador";

	private static void PrintError(String text) {
		// Console.ForegroundColor = ConsoleColor.Red;
		// Console.WriteLine(text);
		// Console.ResetColor();
		MessageBox.Show(text, SUBJECT_NAME, MessageBoxButtons.OK, MessageBoxIcon.Error);
	}

	private DirectoryInfo projectRoot;
	private DirectoryInfo solutionRoot;
	private List<String> files;

	private static string GetRelativePath(string fromPath, string toPath) {
		if (string.IsNullOrEmpty(fromPath))
			throw new ArgumentNullException("fromPath");

		if (string.IsNullOrEmpty(toPath))
			throw new ArgumentNullException("toPath");

		Uri fromUri = new Uri(AppendDirectorySeparatorChar(fromPath));
		Uri toUri = new Uri(AppendDirectorySeparatorChar(toPath));

		if (fromUri.Scheme != toUri.Scheme)
			return toPath;

		Uri relativeUri = fromUri.MakeRelativeUri(toUri);
		string relativePath = Uri.UnescapeDataString(relativeUri.ToString());

		if (string.Equals(toUri.Scheme, Uri.UriSchemeFile, StringComparison.OrdinalIgnoreCase))
			relativePath = relativePath.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);

		return relativePath;
	}

	private static string AppendDirectorySeparatorChar(string path) {
		// Append a slash only if the path is a directory and does not have a slash.
		if (!Path.HasExtension(path) && !path.EndsWith(Path.DirectorySeparatorChar.ToString()))
			return path + Path.DirectorySeparatorChar;

		return path;
	}

	public bool FindProject(DirectoryInfo root) {
		FileInfo[] vcxprojs = root.GetFiles("*.vcxproj");

		if (vcxprojs.Length > 1) {
			PrintError("Hay más de un proyecto de Visual Studio en el directorio actual.");
			return false;
		}

		if (vcxprojs.Length == 0) {
			// Comprueba si es directorio de solución
			FileInfo[] slns = root.GetFiles("*.sln");

			if (slns.Length != 1) {
				PrintError("El programa debe ejecutarse en el directorio del proyecto de Visual Studio.");
				return false;
			}

			solutionRoot = root;

			// Busca el directorio del proyecto
			vcxprojs = root.GetFiles("*.vcxproj", SearchOption.AllDirectories);

			if (vcxprojs.Length != 1) {
				PrintError(vcxprojs.Length == 0
					? "No se encuentra el proyecto de Visual Studio."
					: "Se ha encontrado más de un proyecto de Visual Studio. Ejecuta el programa desde el proyecto que quieras enviar."
				);
				return false;
			}

			projectRoot = vcxprojs[0].Directory;
		}
		else {
			// Busca la solución en el directorio padre
			FileInfo[] slns = root.Parent.GetFiles("*.sln");

			if (slns.Length != 1) {
				PrintError("No se encuentra la solución de Visual Studio (archivo .sln) en el directorio padre.");
				return false;
			}

			projectRoot = root;
			solutionRoot = root.Parent;
		}

		return true;
	}

	private static bool ProjectAllowed(String ext) {
		switch (ext) {
			case ".cpp":
			case ".cc":
			case ".h":
			case ".hpp":
			case ".hh":
			case ".vcxproj":
			case ".filters":
			case ".user":
				return true;
			default:
				return false;
		}
	}

	private static bool SolutionAllowed(String ext) {
		switch (ext) {
			case ".build":
			case ".props":
			case ".sln":
				return true;
			default:
				return false;
		}
	}

	private void CollectFiles() {
		// Archivos de código en el directorio del proyecto
		foreach (FileInfo file in projectRoot.GetFiles()) {
			if (ProjectAllowed(file.Extension.ToLower()))
				files.Add(file.FullName);
		}

		// Archivos de la solución y de propiedades en la raíz de la solución
		foreach (FileInfo file in solutionRoot.GetFiles()) {
			if (SolutionAllowed(file.Extension.ToLower()) && (file.Name != "meson.build" || file.Length != 1169))
				files.Add(file.FullName);
		}

		// Imágenes en la carpeta images
		if (Directory.Exists(solutionRoot.FullName + "/assets/images")) {
			foreach (FileInfo file in solutionRoot.GetFiles("assets/images/*")) {
				files.Add(file.FullName);
			}
		}

		// Shaders en la carpeta shaders
		foreach (FileInfo file in solutionRoot.GetFiles("assets/shaders/*")) {
			files.Add(file.FullName);
		}

		// Comprueba el archivo autores.txt
		FileInfo[] autores = solutionRoot.GetFiles("autores.txt");

		if (autores.Length == 0) {
			PrintError("Falta el archivo «autores.txt» en la raíz de la solución con el nombre de los autores y la indicación de las partes opcionales realizadas.");
		}
		else {
			files.Add(autores[0].FullName);
		}
	}

	public void CreateZip(String outfile) {
		String archiveRoot = solutionRoot.FullName;

		using (FileStream stream = File.OpenWrite(outfile)) {
			using (ZipArchive zip = new ZipArchive(stream, ZipArchiveMode.Create)) {
				foreach (String file in files)
					zip.CreateEntryFromFile(file, GetRelativePath(archiveRoot, file));
			}
		}
	}

	public bool Valid() {
		return projectRoot != null && solutionRoot != null;
	}

	public Submission(DirectoryInfo root) {
		if (FindProject(root)) {
			this.files = new List<String>();
			CollectFiles();
		}
	}

	public static int Main(String[] args) {

		// Base directory
		DirectoryInfo root = null;

		if (args.Length == 0)
			root = new DirectoryInfo(".");

		else if (args.Length == 1)
			root = new DirectoryInfo(args[0]);

		else {
			PrintError("Número de parámetros incorrecto (se espera la ruta de un directorio o nada).");
			return 1;
		}

		if (root != null) {
			Submission subm = new Submission(root);

			if (subm.Valid())
				subm.CreateZip("entrega.zip");
		}

		return 0;
	}
}
