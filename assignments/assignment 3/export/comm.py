#!/usr/bin/python

import sys, locale
from optparse import OptionParser

class comm:
    def __init__(self, file1, file2, options):
        self.__compdlines__ = [[], [], []]
        self.__filelines__ = [[], []]
        self.__is_file1_hidden__ = options.is_file1_hidden
        self.__is_file2_hidden__ = options.is_file2_hidden
        self.__is_common_hidden__ = options.is_common_hidden
        self.__is_unsorted__ = options.is_unsorted
        filenames = [file1, file2]
        for i in range(len(filenames)):
            filename = filenames[i]
            if filename == "-":
                self.__filelines__[i] = sys.stdin.readlines()
            else:
                f = open(filename, 'r')
                self.__filelines__[i] = f.readlines()
                f.close()


    def compare(self):
        if self.__is_unsorted__:
            self.__compare_unsorted_lines__()
        else:
            self.__compare_sorted_lines__()
        return self.__format_compared_lines__()

    def __compare_unsorted_lines__(self):
        file1_lines = self.__filelines__[0]
        file2_lines = self.__filelines__[1]
        uniq_file2_lines = list(file2_lines)
        for file1_line in file1_lines:
            if file1_line not in uniq_file2_lines:
                self.__add_word_and_placeholders__(0, file1_line)
            else:
                self.__add_word_and_placeholders__(2, file1_line)
                uniq_file2_lines.remove(file1_line)

        for uniq_file2_line in uniq_file2_lines:
            self.__add_word_and_placeholders__(1, uniq_file2_line)

    def __compare_sorted_lines__(self):
        file1_lines = self.__filelines__[0]
        file2_lines = self.__filelines__[1]
        i, j = 0, 0

        while i < len(file1_lines) and j < len(file2_lines):
            file1_curline = file1_lines[i]
            file2_curline = file2_lines[j]
            if locale.strcoll(file1_curline,file2_curline) == 0:
                self.__add_word_and_placeholders__(2, file1_curline)
                i += 1
                j += 1
            elif locale.strcoll(file1_curline, file2_curline) > 0:
                self.__add_word_and_placeholders__(1, file2_curline)
                j += 1
            elif locale.strcoll(file1_curline, file2_curline) < 0:
                self.__add_word_and_placeholders__(0, file1_curline)
                i += 1

        while i < len(file1_lines):
            self.__add_word_and_placeholders__(0, file1_lines[i])
            i += 1

        while j < len(file2_lines):
            self.__add_word_and_placeholders__(1, file2_lines[j])
            j += 1

    def __add_word_and_placeholders__(self, col, word):
        for i in range(len(self.__compdlines__)):
            if i == col:
                self.__compdlines__[i].append(word)
            else:
                self.__compdlines__[i].append("")

    def __format_compared_lines__(self):
        ostream = ""
        for i in range(len(self.__compdlines__[0])):
            file1word = self.__compdlines__[0][i]
            file2word = self.__compdlines__[1][i]
            commonword = self.__compdlines__[2][i]
            if not self.__is_file1_hidden__ and file1word != "":
                ostream += file1word
            if not self.__is_file2_hidden__ and file2word != "":
                lead = "" if self.__is_file1_hidden__ else "\t"
                ostream += (lead + file2word)
            if not self.__is_common_hidden__ and commonword != "":
                lead = "\t\t"
                if self.__is_file1_hidden__ and self.__is_file2_hidden__:
                    lead = ""
                elif self.__is_file1_hidden__ or self.__is_file2_hidden__:
                    lead = "\t"
                ostream += (lead + commonword)
        return ostream

def main():
    version_msg = "%prog 1.0"
    usage_msg = """%prog [OPTION] ... FILE1 FILE2

Compares files FILE1 FILE2 line by line.
"""
    parser = OptionParser(version=version_msg,
                          usage=usage_msg)
    parser.add_option("-1", "--hidecolumn1",
                    action="store_true", dest="is_file1_hidden",
                    default=False, help="Suppress printing of column 1 (lines unique to FILE1)")
    parser.add_option("-2", "--hidecolumn2",
                    action="store_true", dest="is_file2_hidden",
                    default=False, help="Suppress printing of column 2 (lines unique to FILE1)")
    parser.add_option("-3", "--hidecolumn3",
                    action="store_true", dest="is_common_hidden",
                    default=False, help="Suppress printing of column 3 (lines common to both FILE1 and FILE2)")
    parser.add_option("-u", "--unsorted",
                    action="store_true", dest="is_unsorted",
                    default=False, help="Sort inputs in ASCII order before comparison")
    options, filenames = parser.parse_args(sys.argv[1:])

    if len(filenames) != 2:
        parser.error("Invalid number of operands")
    if filenames[0] == filenames[1] == "-":
        parser.error("Only one file can be stdin")
    try:
        comparator = comm(filenames[0], filenames[1], options)
        sys.stdout.write(comparator.compare())
    except IOError as err:
        parser.error("I/O error({0}): {1}".
                     format(err.errno, err.strerror))

if __name__ == "__main__":
    main()
