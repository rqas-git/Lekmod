from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import MagicMock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import google_drive_api as downloads


class DownloadTests(unittest.TestCase):
    def response(self, html=False, cookie=False):
        response = MagicMock()
        response.__enter__.return_value = response
        response.headers = {'content-type': 'text/html' if html else 'application/zip',
                            'content-length': '7'}
        response.cookies = {'download_warning': 'token'} if cookie else {}
        response.status_code = 200
        response.iter_content.return_value = [b'payload']
        response.content = b'<html>quota exceeded</html>'
        return response

    def run_download(self, responses, expect_error=None):
        with tempfile.TemporaryDirectory() as folder:
            session = MagicMock()
            session.__enter__.return_value = session
            session.get.side_effect = responses
            with patch.object(downloads.requests, 'Session', return_value=session):
                operation = lambda: downloads.GoogleDriveDownloader({})._download_file(
                    'fixture', '35.3', lambda _: None, download_dir=folder)
                if expect_error:
                    with self.assertRaises(expect_error):
                        operation()
                else:
                    self.assertEqual(Path(operation()).read_bytes(), b'payload')
            self.assertTrue(session.get.called)
            for call in session.get.call_args_list:
                self.assertEqual(call.kwargs['timeout'], (10, 60))
                self.assertTrue(call.kwargs['stream'])
            session.__exit__.assert_called_once()
            for response in responses[:session.get.call_count]:
                if not isinstance(response, Exception):
                    response.__exit__.assert_called_once()
            return session

    def test_direct_cookie_confirmation_and_fallback_requests_are_bounded(self):
        cases = [
            [self.response()],
            [self.response(html=True, cookie=True), self.response()],
            [self.response(html=True), self.response()],
            [self.response(html=True) for _ in range(5)] + [self.response()],
        ]
        for responses in cases:
            with self.subTest(requests=len(responses)):
                session = self.run_download(responses)
                self.assertEqual(session.get.call_count, len(responses))
                for response in responses[:-1]:
                    response.close.assert_called()

    def test_timeout_at_each_request_stage_closes_resources(self):
        cases = [[], [self.response(html=True, cookie=True)],
                 [self.response(html=True)], [self.response(html=True) for _ in range(5)]]
        for responses in cases:
            with self.subTest(request=len(responses)+1):
                self.run_download(responses + [downloads.requests.Timeout('server stalled')],
                                  downloads.requests.Timeout)

    def test_stream_timeout_http_error_and_short_body_fail(self):
        timeout = self.response()
        def chunks(**kwargs):
            yield b'part'
            raise downloads.requests.ReadTimeout('stream stalled')
        timeout.iter_content.side_effect = chunks
        http = self.response()
        http.raise_for_status.side_effect = downloads.requests.HTTPError('503 unavailable')
        truncated = self.response()
        truncated.headers['content-length'] = '100'
        for response, error in ((timeout, downloads.requests.ReadTimeout),
                                (http, downloads.requests.HTTPError), (truncated, RuntimeError)):
            with self.subTest(error=error):
                self.run_download([response], error)

    def test_html_quota_response_is_not_saved_as_a_release(self):
        self.run_download([self.response(html=True) for _ in range(6)], Exception)
