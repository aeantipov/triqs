class Triqs < Formula
  desc "Applications and Libraries for Physics Simulations"
  
  homepage "http://ipht.cea.fr/triqs/"
  url 'https://github.com/TRIQS/triqs/archive/1.3.tar.gz'
  sha256 "b5b7aed57f3c4b08d1da72b9590410d4ccf19e0249bf191a5ef332f83678d976"
  head "https://github.com/TRIQS/triqs.git"
  version '1.3'

  option "with-test",   "Build and run shipped tests"
  option "with-doc",    "Build documentation"
  option "with-static", "Build static instead of shared libraries"

  depends_on "cmake" => :build
  depends_on :mpi => [:cc, :cxx, :recommended]

  depends_on "boost"
  depends_on "hdf5"
  
  depends_on 'cmake' => :build
  #depends_on 'gfortran'
  depends_on 'open-mpi'
  depends_on 'fftw'
  depends_on 'gmp'
  #depends_on 'gsl'
  depends_on 'python'

  depends_on :python => 'numpy'
  depends_on :python => 'scipy'
  depends_on :python => 'h5py'
  depends_on :python => 'matplotlib'
  depends_on :python => 'ipython'
  depends_on :python => 'jupyter'

  def install
    ENV.cxx11 
    args = std_cmake_args
    args.delete "-DCMAKE_BUILD_TYPE=None"
    args << "-DCMAKE_BUILD_TYPE=Release"

    #args << ("-DDocumentation=" + ((build.with? "doc") ? "ON" : "OFF"))
    #args << ("-DTesting=" + ((build.with? "test") ? "ON" : "OFF"))

    args << ["-DPYTHON_INTERPRETER=/usr/local/bin/python", "-DALLOW_COMPILATION_IN_SOURCES=ON" , "-DCMAKE_INSTALL_PREFIX=/usr/local"]
    
    mkdir "tmp" do
      args << ".."
      system "pip install --upgrade h5py"
      system "cmake", *args
      system "make -j8"
      system "make", "test" if build.with? "test"
      system "make -j8 install"
    end
  end


  test do
    system "python -c 'import pytriqs'"
  end
end


