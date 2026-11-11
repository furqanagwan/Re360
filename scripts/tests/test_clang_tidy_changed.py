import importlib.util
import io
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location(
    'clang_tidy_changed', Path(__file__).parents[1] / 'clang_tidy_changed.py')
tidy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(tidy)


class ClangTidyChangedTests(unittest.TestCase):
    def test_added_ranges_skip_deletions_and_deleted_files(self):
        diff = '\n'.join([
            '+++ b/src/core/example.cpp',
            '@@ -2 +2,3 @@',
            '@@ -10,2 +12,0 @@',
            '@@ -20 +20 @@',
            '+++ /dev/null',
            '@@ -1,4 +0,0 @@',
            '+++ b/src/core/other.cpp',
            '@@ -0,0 +1,2 @@',
        ])
        self.assertEqual(tidy.changed_lines(diff), {
            'src/core/example.cpp': [[2, 4], [20, 20]],
            'src/core/other.cpp': [[1, 2]],
        })

    def test_filter_uses_native_absolute_path(self):
        path = (Path.cwd() / 'src/core/file with spaces.cpp').resolve()
        command = tidy.tidy_command('clang-tidy', Path('build'), path, [[4, 7]])
        line_filter = next(arg for arg in command if arg.startswith('-line-filter='))
        self.assertEqual(json.loads(line_filter.split('=', 1)[1]), [
            {'name': str(path), 'lines': [[4, 7]]},
        ])
        self.assertIn('-warnings-as-errors=*', command)
        self.assertEqual(command[-1], str(path))

    def run_check(self, return_code, diff):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory)
            source = (Path.cwd() / 'src/core/example.cpp').resolve()
            (build / 'compile_commands.json').write_text(json.dumps([
                {'file': str(source)}, {'file': str(source)},
            ]), encoding='utf-8')
            output = io.StringIO()
            result = subprocess.CompletedProcess([], return_code, 'diagnostic\n', '')
            with patch('sys.argv', ['clang_tidy_changed.py', '--build', str(build)]), \
                    patch('sys.stdin', io.StringIO(diff)), \
                    patch('sys.stdout', output), \
                    patch.object(tidy.subprocess, 'run', return_value=result) as run:
                status = tidy.main()
            return status, output.getvalue(), run.call_count

    def test_regpu_renderer_is_left_out_but_its_plugin_is_checked(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory)
            renderer = (Path.cwd() / 'src/regpu/gpu/renderer.cpp').resolve()
            plugin = (Path.cwd() / 'src/regpu/plugin/graphics_system.cpp').resolve()
            (build / 'compile_commands.json').write_text(json.dumps([
                {'file': str(renderer)}, {'file': str(plugin)},
            ]), encoding='utf-8')
            diff = '\n'.join([
                '+++ b/src/regpu/gpu/renderer.cpp', '@@ -1 +1 @@',
                '+++ b/src/regpu/plugin/graphics_system.cpp', '@@ -1 +1 @@',
            ])
            result = subprocess.CompletedProcess([], 0, '', '')
            with patch('sys.argv', ['clang_tidy_changed.py', '--build', str(build)]), \
                    patch('sys.stdin', io.StringIO(diff)), \
                    patch('sys.stdout', io.StringIO()), \
                    patch.object(tidy.subprocess, 'run', return_value=result) as run:
                self.assertEqual(tidy.main(), 0)
            checked = [call.args[0][-1] for call in run.call_args_list]
            self.assertEqual(checked, [str(plugin)])

    def test_diagnostic_failure_propagates_once_per_file(self):
        status, output, calls = self.run_check(
            1, '+++ b/src/core/example.cpp\n@@ -0,0 +1 @@\n')
        self.assertEqual(status, 1)
        self.assertEqual(calls, 1)
        self.assertIn('diagnostic', output)
        self.assertIn('1 with findings', output)

    def test_clean_changed_file_passes(self):
        status, output, calls = self.run_check(
            0, '+++ b/src/core/example.cpp\n@@ -0,0 +1 @@\n')
        self.assertEqual(status, 0)
        self.assertEqual(calls, 1)
        self.assertIn('0 with findings', output)

    def test_uncompiled_or_excluded_files_are_skipped(self):
        status, output, calls = self.run_check(1, '\n'.join([
            '+++ b/include/rex/example.h', '@@ -0,0 +1 @@',
            '+++ b/thirdparty/example.cpp', '@@ -0,0 +1 @@',
        ]))
        self.assertEqual(status, 0)
        self.assertEqual(calls, 0)
        self.assertIn('No changed lines', output)


if __name__ == '__main__':
    unittest.main()
