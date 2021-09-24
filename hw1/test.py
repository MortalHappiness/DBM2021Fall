import unittest
import subprocess
import os
import sys


class Test(unittest.TestCase):
    def setUp(self):
        self.maxDiff = None
        self.script_name = "./main"

    def test_01_sample(self):
        input_data = "user_missions.csv\n2021-02-22\n15\n"
        ans = "266,54\n153,52\n262,52\n143,50\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_02_sample(self):
        input_data = "user_missions.csv\n2021-02-22\n30\n"
        ans = "36,53\n48,51\n492,53\n248,51\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_03_sample(self):
        input_data = "user_missions.csv\n2021-01-01\n15\n"
        ans = "196,51\n165,50\n271,55\n152,53\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_04_sample(self):
        input_data = "user_missions.csv\n2021-01-01\n30\n"
        ans = "25,45\n20,50\n442,53\n297,51\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_05_sample(self):
        input_data = "user_missions.csv\n2020-10-01\n2\n"
        ans = "477,50\n290,50\n71,60\n14,73\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_06_sample(self):
        input_data = "user_missions.csv\n2020-10-01\n100\n"
        ans = "0,0\n0,0\n548,51\n304,51\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_07_sample(self):
        input_data = "user_missions_another.csv\n2021-02-22\n15\n"
        ans = "273,55\n127,52\n259,51\n156,50\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_08_online_sample(self):
        input_data = "user_missions_another.csv\n2021-02-22\n100\n"
        ans = "1,78\n2,52\n531,53\n281,51\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)

    def test_09_online_sample(self):
        input_data = "user_missions_another.csv\n2021-02-22\n30\n"
        ans = "34,55\n43,50\n498,53\n240,51\n"
        output = subprocess.check_output(
            [self.script_name], text=True, input=input_data)
        self.assertEqual(output, ans)


if __name__ == "__main__":
    unittest.main()
